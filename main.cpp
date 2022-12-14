#include <iostream>
#include <pthread.h>
#include <semaphore.h>

int maxThreadAmount = 3;
int currentThreadAmount = 0;
//sem_t threadAmountSemaphore;
pthread_mutex_t threadAmountMutex;


class Thread {
public:
    virtual ~Thread() = default;
    int start() {
        return pthread_create(&_ThreadId, nullptr, Thread::threadFunction, this);
    }
    int wait() {
        return pthread_join(_ThreadId, nullptr);
    }
protected:
    pthread_t  _ThreadId;
    virtual void logic () = 0;
    static void* threadFunction(void* d) {
        (static_cast<Thread*>(d))->logic();
        return nullptr;
    }
};

double f(double x) {
    return 0.04 * x * x - 3 * x + 1;
}


double approximate(double left, double right) {
    double res = 0;
    double piece_length = (right - left) / 1000;
    for (int i = 0; i < 1000; ++i) {
        res += f(left + i * piece_length) * piece_length;
    }
    return res;
}

class CalculationThread : public Thread {
public:
    CalculationThread(double a, double b, double precision) : a(new double(a)), b(new double(b)), precision(new double(precision)), res(new double(-1)) {
    }

    double getResult() const {
        return *res;
    }

    ~CalculationThread() {
        delete a;
        delete b;
        delete precision;
        delete res;
    }

private:
    double *a, *b, *precision, *res;

    void logic() {
        double m = (*a + *b) / 2;
        double halfPrecision = *precision / 2;
        bool doLeftInThisThread, doRightInThisThread;

        pthread_mutex_lock(&threadAmountMutex);

        CalculationThread leftThread(*a, m, halfPrecision);
        if (!(doLeftInThisThread = (currentThreadAmount >= maxThreadAmount))) {
            currentThreadAmount++;
            leftThread.start();
        }

        CalculationThread rightThread(m, *b, halfPrecision);
        if (!(doRightInThisThread = (currentThreadAmount >= maxThreadAmount))) {
            currentThreadAmount++;
            rightThread.start();
        }

        pthread_mutex_unlock(&threadAmountMutex);


        double approximation = approximate(*a, *b);
        double leftApproximation = approximate(*a, m);
        double rightApproximation = approximate(m, *b);
        double diff = leftApproximation + rightApproximation - approximation;
        diff = (diff < 0) ? -diff : diff;
        if (diff < *precision) {
            *res = approximation;
            return;
        }

        if (doLeftInThisThread) {
            leftThread.logic();
        } else {
            leftThread.wait();
            pthread_mutex_lock(&threadAmountMutex);
            currentThreadAmount--;
            pthread_mutex_unlock(&threadAmountMutex);
        }

        if (doRightInThisThread) {
            rightThread.logic();
        } else {
            rightThread.wait();
            pthread_mutex_lock(&threadAmountMutex);
            currentThreadAmount--;
            pthread_mutex_unlock(&threadAmountMutex);
        }

        *res = leftThread.getResult() + rightThread.getResult();
    }


};

int main() {
    CalculationThread myThread(-5, 4, 0.01);

    pthread_mutex_lock(&threadAmountMutex);
    myThread.start();
    currentThreadAmount++;
    pthread_mutex_unlock(&threadAmountMutex);

    myThread.wait();
    std::cout << "res: " << myThread.getResult() << "\n";
}
