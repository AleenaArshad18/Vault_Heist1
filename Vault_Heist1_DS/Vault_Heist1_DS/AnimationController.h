
#pragma once

#include <string>
using namespace std;

class AnimationController
{
private:
    static const int MAX_EVENTS = 100;

    string events[MAX_EVENTS];
    int eventCount = 0;

    // 0 means the initial state.
    // 1 means the state after event 1, and so on.
    int currentIndex = 0;

    bool playing = false;

    float speed = 1.0f;
    float elapsedTime = 0.0f;
    float eventDuration = 0.75f;

public:
    AnimationController();

    void setEvents(const string newEvents[], int count);

    void play();
    void pause();
    void togglePlayPause();

    void stepForward();
    void reset();

    void update(float deltaTime);

    void setSpeed(float newSpeed);

    bool isPlaying() const;
    bool isFinished() const;

    int getCurrentIndex() const;
    int getEventCount() const;

    float getSpeed() const;

    string getCurrentEvent() const;
};
