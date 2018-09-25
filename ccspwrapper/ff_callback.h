#ifndef CCSP_FF_CALLBACK_H_INCLUDED
#define CCSP_FF_CALLBACK_H_INCLUDED

extern "C" {
  typedef struct vacall_alist * va_alist; //from vcall.h
}

namespace ccspwrapper {

class FfCallback {
public:
  struct Callable {
    virtual ~Callable() {}
    virtual void OnFfCallback(FfCallback& caller, va_alist alist) = 0;
  };

  FfCallback(Callable& callable);
  virtual ~FfCallback();

  void *GetTramp() const;


private:
  FfCallback(const FfCallback&); //no copy... for now...

  static void _c_ffCallback(void *data, va_alist alist);

  Callable&      _callable;
  void * const   _ffTramp;

};


}; //namespace ccspwrapper




#endif //#ifndef CCSP_FF_CALLBACK_H_INCLUDED
