#include <iostream>
#include <pthread.h>

int maxThreadAmount = 4;
int currentThreadAmount = 0;
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


double a = 1;
double b = 2;
double c = 3;
double f(double x) {
    return a * x * x + b * x + c;
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
    try {
    std::cout << "enter max amount of threads (1 <= t <= 99 | default=4)\n";
    int t;
    std::cin >> t;
    if (t >= 1 && t <= 99) {
        maxThreadAmount = t;
    } else {
        std::cout << "using default value = 4\n";
    }
    std::cout << "enter coefficients of polynomet ax^2+bx+c\n";
    double myA, myB, myC;
    std::cin >> myA >> myB >> myC;
    a = myA;
    b = myB;
    c = myC;
    std::cout << "enter left border, right border and precision\n(left < right; precision > 0.00001\ndefault values: -2 2 0.001)\n";
    double left = -2, right = 2, precision = 0.00001;
    std::cin >> left >> right >> precision;
    if (left >= right) {
        left = -2;
        right = 2;
        std::cout << "using default values for left and right\n";
    }
    if (precision < 0.00001) {
        precision = 0.001;
        std::cout << "using default value for precision\n";
    }
    CalculationThread myThread(left, right, precision);


    pthread_mutex_lock(&threadAmountMutex);
    myThread.start();
    currentThreadAmount++;
    pthread_mutex_unlock(&threadAmountMutex);

    myThread.wait();
    std::cout << "result: " << myThread.getResult() << "\n";
    } catch (std::exception& e) {
        std::cout << "invalid input\n";
    }
}
