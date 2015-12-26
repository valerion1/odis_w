#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class Singleton
{
	static T* _self;
	static int _refcount;
protected:
	Singleton(){}
	virtual ~Singleton(){_self=NULL;}
public:
	static T* createInstance();
	void freeInst();
};

template <class T>
T*  Singleton<T>::_self = NULL;

template <class T>
int  Singleton<T>::_refcount=0;

template <class T>
T*  Singleton<T>::createInstance()
{
	if(!_self)
		_self=new T;
	_refcount++;
	return _self;
}

template <class T>
void  Singleton<T>::freeInst()
{
	if(--_refcount==0)
		delete this;
}

#endif //SINGLETON_H