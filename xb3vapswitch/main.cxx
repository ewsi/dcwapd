

#include "ccspwrapper/tr181_sub_collection_provider.h"
#include "ccspwrapper/tr181_collection_registry.h"
#include "ccspwrapper/tr181_config_provider.h"
#include "ccspwrapper/ccsp_abstractor.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string>
#include <map>


typedef std::map<std::string, int> OriginalVlanMap;
static OriginalVlanMap _origVlanMap;
static int _dcwdataVlan;


class IwPrivCmd {
  static bool run(std::string& output, const char * const ifName, const char * const cmd, const char * const value = NULL) {
    char *argv[] = {
      (char*)"iwpriv",
      (char*)ifName, (char*)cmd, (char*)value, NULL,
    };
    pid_t child;
    int status;
    int stdout_pipe[2];
    char buf[64];
    ssize_t readsize;

    if (pipe(stdout_pipe) == -1) {
      perror("pipe() failed");
      return false; //failed to execute
    }

    switch(child = fork()) {
    case -1:
      perror("fork() failed");
      return false; //failed to execute
    case 0:
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
      dup2(stdout_pipe[1], STDOUT_FILENO);
      close(stdout_pipe[0]);
      close(stdout_pipe[1]);
      execvp(argv[0], argv);
      perror("exec() failed");
      _exit(1); //shouldnt get here...
    default:
      break;
    }
    close(stdout_pipe[1]);
    while ((readsize = read(stdout_pipe[0], buf, sizeof(buf))) > 0) {
      output.append(buf, readsize);
    }
    close(stdout_pipe[0]);
    waitpid(child, &status, 0);

    if (!WIFEXITED(status)) return false;
    if (WEXITSTATUS(status) != 0) return false;

    return true;
  }
public:
  static int GetVlanId(const char * const ifName) {
    std::string cmdOutput;
    if (!run(cmdOutput, ifName, "get_vlanID")) return -1;
    size_t pos = cmdOutput.find_first_of(':');
    if (pos == std::string::npos) return -1;
    return atoi(cmdOutput.c_str() + pos + 1);
  }

  static bool SetVlanId(const char * const ifName, unsigned vlanId) {
    std::string cmdOutput;
    char vlanIdStr[16];
    if ((vlanId < 1) || (vlanId > 4095)) return false;
    snprintf(vlanIdStr, sizeof(vlanIdStr), "%u", vlanId);
    return run(cmdOutput, ifName, "vlanID", vlanIdStr);
  }
};


class SingleInterfaceProvider : public ::ccspwrapper::Tr181ConfigProvider {
  std::string _ifName;

  int getAndUpdateOriginalVlanId() {
    int currentVlanId = IwPrivCmd::GetVlanId(_ifName.c_str());
    if ((currentVlanId >= 1) && (currentVlanId != _dcwdataVlan)) {
      _origVlanMap[_ifName] = currentVlanId;
    }
    return currentVlanId;
  }

public:
  SingleInterfaceProvider(const char * const ifName) : _ifName(ifName) {}
  virtual ~SingleInterfaceProvider() { }

  virtual void GetValue(const char * const name, ccspwrapper::Tr181Scalar& value) {
    if (strcmp(name, "Name") == 0) {
      value = _ifName;
    }
    else if (strcmp(name, "DataEnable") == 0) {
      value = getAndUpdateOriginalVlanId() == _dcwdataVlan;
    }
    else {
      throw ::ccspwrapper::Tr181Exception();
    }
  }
  virtual void SetValue(const char * const name, const ccspwrapper::Tr181Scalar& value) {
    if (strcmp(name, "DataEnable") == 0) {
      //const int currentVlanId = getAndUpdateOriginalVlanId();
      getAndUpdateOriginalVlanId();

      OriginalVlanMap::const_iterator origVlan = _origVlanMap.find(_ifName);
      if (origVlan == _origVlanMap.end()) {
        if (!((bool)value)) {
          //we dont have an original vlan backed up for this interface, but are being asked
          //to turn off dcw data on this so this is essentiall a no-op...
          return;
        }
        throw ::ccspwrapper::Tr181Exception(); //we should hace an original vlan id by this point... if not, then the interface is probably not ready yet...
      }

      const int newVlanId = ((bool)value) ? _dcwdataVlan : origVlan->second;

      if (!IwPrivCmd::SetVlanId(_ifName.c_str(), newVlanId))
        throw ::ccspwrapper::Tr181Exception();
    }
    else {
      throw ::ccspwrapper::Tr181Exception();
    }
  }

};

class ConfigProvider :
  public ::ccspwrapper::AutoDeleteTr181SubCollectionProvider,
  public ::ccspwrapper::Tr181ConfigProvider
  {

public:
  ConfigProvider() :
    _ccspAbstractor("xb3vapswitch.ccsp.cfg.xml", *this) { }


  virtual void GetSubCollections(ccspwrapper::Tr181CollectionRegistry& registry) {
    registry.DeclareSubCollectionProvider("Interface", *this);
  }
  virtual void PopulateConfigProviderCollection(::ccspwrapper::Tr181ConfigProvider& parent, ConfigProviderCollection& collection) {
    DIR            *dirp;
    struct dirent   de, *dep;


    //hacky, but to guarantee the order, find the highest
    //device number and insert into the collection based
    //on that whether or not the device exists...
    int high_ifnum = -1;

    dirp = opendir("/sys/class/net");
    if (dirp == NULL)  throw ::ccspwrapper::Tr181Exception();
    while (readdir_r(dirp, &de, &dep) == 0) {
      if (dep == NULL) break;
      if (strncmp(de.d_name, "ath", 3) != 0) continue;
      int ifnum = atoi(de.d_name + 3);
      if (ifnum > high_ifnum) high_ifnum = ifnum;
    }
    closedir(dirp);

    char ifname[16];
    for (int i = 0; i <= high_ifnum; ++i) {
      snprintf(ifname, sizeof(ifname), "ath%d", i);
      collection.push_back(new SingleInterfaceProvider(ifname));
    }

  }
  virtual void GetValue(const char * const name, ccspwrapper::Tr181Scalar& value) {
    throw ::ccspwrapper::Tr181Exception();
  }


private:
  //declare this thing last so that we initialize all our stuff 
  //before this will fire up tr181 on construction...
  ccspwrapper::CcspAbstractor _ccspAbstractor;
};

int main() {
  try {
    _dcwdataVlan = 4089; //XXX
    ConfigProvider cp;

    while ( 1 ) sleep(10);
  }
  catch (std::exception& e) {
    fprintf(stderr, "Caught Exception: %s\n", e.what());
  }
  return 0;
}
