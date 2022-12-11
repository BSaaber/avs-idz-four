#include <iostream>
#include <pthread.h>

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

class LeftPrinterThread : public Thread {
    void logic() override {
        while(true) {
            std::cout << "(";
        }

    }
};

class RightPrinterThread : public Thread {
    void logic() override {
        while (true) {
            std::cout << ")";
        }
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
        CalculationThread leftThread(*a, m, halfPrecision);
        CalculationThread rightThread(m, *b, halfPrecision);
        leftThread.start();
        rightThread.start();
        double approximation = approximate(*a, *b);
        double leftApproximation = approximate(*a, m);
        double rightApproximation = approximate(m, *b);
        double diff = leftApproximation + rightApproximation - approximation;
        diff = (diff < 0) ? -diff : diff;
        if (diff < *precision) {
            *res = approximation;
            return;
        }
        leftThread.wait();
        rightThread.wait();
        *res = leftThread.getResult() + rightThread.getResult();
    }


};

int main() {
    CalculationThread myThread(-5, 4, 0.001);
    myThread.start();
    myThread.wait();
    std::cout << "res: " << myThread.getResult() << "\n";
}
