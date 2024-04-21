#ifndef SINGLETON_H
#define SINGLETON_H

#include <QMutex>
#include <QMutexLocker>

template <typename T> class Singleton
{
public:
    static T* getInstance() {
        QMutexLocker locker(&mutex);
        (void)locker;

        if (instance == 0)
            instance = new T;

        return instance;
    }

    static void destroyInstance() {
        delete instance;
        instance = 0;
    }

protected:
    Singleton() { }
    virtual ~Singleton() { }

private:
    Singleton(const Singleton& source) { (void)source; }
    Singleton& operator = (const Singleton&);

    static QMutex mutex;
    static T* instance;
};

template <typename T> T* Singleton<T>::instance = 0;
template <typename T> QMutex Singleton<T>::mutex;

#endif // SINGLETON_H
