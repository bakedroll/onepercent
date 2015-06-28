#include <osgGaming/Timer.h>

using namespace osgGaming;

Timer::Timer()
	: Referenced(),
	  _duration(1.0),
	  _singleShot(true),
	  _started(false),
	  _stopped(false),
	  _running(false)
{

}

Timer::Timer(Callback callback, double duration, bool singleShot)
	: Referenced(),
	  _callback(callback),
	  _duration(duration),
	  _singleShot(singleShot),
	  _started(false),
	  _stopped(false),
	  _running(false)
{

}

bool Timer::update(double time)
{
	if (_started)
	{
		_timeStarted = time;
		_running = true;
		_started = false;
	}

	if (_stopped)
	{
		_running = false;
		_stopped = false;

		return true;
	}

	if (_running)
	{
		if (time - _timeStarted >= _duration)
		{
			_callback();

			if (_singleShot)
			{
				_running = false;

				return true;
			}
			else
			{
				_timeStarted = time;
			}
		}
	}

	return false;
}

void Timer::start()
{
	_started = true;
}

void Timer::stop()
{
	_stopped = true;
}

void Timer::setDuration(double duration)
{
	_duration = duration;
}

void Timer::setSingleShot(bool singleShot)
{
	_singleShot = singleShot;
}

void Timer::setCallback(Callback callback)
{

}

bool Timer::running()
{
	return _running;
}
