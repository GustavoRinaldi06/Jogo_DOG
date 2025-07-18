#ifndef TIMER
#define TIMER

class Timer
{
    public:
        Timer();
        void Update(float dt);
        void Restart();
        float Get() const;
        void Set(float dt);
        bool Between(float min, float max) const;

    private:
        float time;
};

#endif // Timer