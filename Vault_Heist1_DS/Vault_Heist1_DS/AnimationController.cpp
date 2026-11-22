
#include "AnimationController.h"

AnimationController::AnimationController()
{
    eventCount = 0;
    currentIndex = 0;
    playing = false;
    speed = 1.0f;
    elapsedTime = 0.0f;
    eventDuration = 0.75f;
}

void AnimationController::setEvents(const string newEvents[], int count)
{
    eventCount = count;

    if (eventCount > MAX_EVENTS)
    {
        eventCount = MAX_EVENTS;
    }

    if (eventCount < 0)
    {
        eventCount = 0;
    }

    for (int i = 0; i < eventCount; i++)
    {
        events[i] = newEvents[i];
    }

    currentIndex = 0;
    elapsedTime = 0.0f;
    playing = false;
}

void AnimationController::play()
{
    if (eventCount == 0)
    {
        return;
    }

    if (currentIndex >= eventCount)
    {
        currentIndex = 0;
        elapsedTime = 0.0f;
    }

    playing = true;
}

void AnimationController::pause()
{
    playing = false;
}

void AnimationController::togglePlayPause()
{
    if (playing)
    {
        pause();
    }
    else
    {
        play();
    }
}

void AnimationController::stepForward()
{
    playing = false;
    elapsedTime = 0.0f;

    if (currentIndex < eventCount)
    {
        currentIndex++;
    }
}

void AnimationController::reset()
{
    currentIndex = 0;
    elapsedTime = 0.0f;
    playing = false;
}

void AnimationController::update(float deltaTime)
{
    if (!playing || currentIndex >= eventCount)
    {
        return;
    }

    elapsedTime += deltaTime;

    float timePerEvent = eventDuration / speed;

    while (elapsedTime >= timePerEvent && currentIndex < eventCount)
    {
        elapsedTime -= timePerEvent;
        currentIndex++;
    }

    if (currentIndex >= eventCount)
    {
        playing = false;
        elapsedTime = 0.0f;
    }
}

void AnimationController::setSpeed(float newSpeed)
{
    if (newSpeed < 0.25f)
    {
        speed = 0.25f;
    }
    else if (newSpeed > 3.0f)
    {
        speed = 3.0f;
    }
    else
    {
        speed = newSpeed;
    }
}

bool AnimationController::isPlaying() const
{
    return playing;
}

bool AnimationController::isFinished() const
{
    return eventCount > 0 && currentIndex >= eventCount;
}

int AnimationController::getCurrentIndex() const
{
    return currentIndex;
}

int AnimationController::getEventCount() const
{
    return eventCount;
}

float AnimationController::getSpeed() const
{
    return speed;
}

string AnimationController::getCurrentEvent() const
{
    if (currentIndex == 0)
    {
        return "INITIAL STATE";
    }

    if (currentIndex > 0 && currentIndex <= eventCount)
    {
        return events[currentIndex - 1];
    }

    return "";
}
