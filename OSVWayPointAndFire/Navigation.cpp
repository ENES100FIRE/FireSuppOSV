#include "Arduino.h"
#include "Navigation.h"
#include "math.h"

Navigator::Navigator(Location* loc, Sonic *sleft, Sonic *smiddle, Sonic *sright, Motor* left, Motor* right)

{
  _leftm = left;
  _rightm = right;
  _sleft = sleft;
  _smiddle = smiddle;
  _sright = sright;
  location = loc;
}

float Navigator::getAngle(float x1, float y1, float x2, float y2)
{
  float dx = (x2 - x1);
  float dy = (y2 - y1);
  if (fabs(dx) < EPSILON)
  {
    return (dy > 0) ? (PI / 2) : (-PI / 2);
  }
  else if (fabs(dy) < EPSILON)
  {
    return (dx > 0) ? 0 : PI;
  }
  else if (dx >= 0 && dy >= 0)
  {
    //q1
    return atan(dy / dx);
  }
  else if (dx <= 0 && dy >= 0)
  {
    //q2
    return PI / 2 + atan(-dx / dy);
  }
  else if (dx >= 0 && dy <= 0)
  {
    //q4
    return atan(dy / dx);
  }
  else
  {
    //q3
    return -(PI / 2 + atan(dx / dy));
  }

}

float getDistance(float x1, float y1, float x2, float y2)
{
  return sqrt(sq(x2 - x1) + sq(y2 - y1));
}

void Navigator::gotoWaypoint(float x, float y)
{
  /*
     x /= 1000;
     y /= 1000;
  */


  location->say("Going to waypoint at (");
  location->say(x);
  location->say(", ");
  location->say(y);
  location->say(")\n");


  float lx = location->getX();
  float ly = location->getY();
  float targetAngle = getAngle(lx, ly, x, y);
  while (getDistance(x, y, lx, ly) > DISTANCE_RANGE)
  {
    //  location->say("Target Angle DEG: ");
    // location->say(targetAngle*180/PI);
    // location->say("\n");
    //location->say("Current Location: ");
    //location->say(lx);
    //location->say(ly);
    //location->say("\n");

    if (fabs(location->getAngle() - targetAngle) > ANGLE_RANGE)
    {
      this->rotateToAngle(targetAngle);
    }
    int speed = 255;
    if (getDistance(x, y, lx, ly) < WAYPOINT_SLOWDOWN_RANGE) {
      speed *= (3.0 / 4.0);
    }
    _leftm->setSpeed(speed, 0);
    _rightm->setSpeed(speed, 0);
    lx = location->getX();
    ly = location->getY();
    targetAngle = getAngle(lx, ly, x, y);
  }
  _leftm->setSpeed(0, 0);
  _rightm->setSpeed(0, 0);

}

float fabs(float f)
{
  return (f < 0) ? (-f) : f;
}

void Navigator::rotateToAngle(float angle)
{

  float ca = location->getAngle();
  float error = 0;
  error = ca - angle;

  if (fabs(error) > PI) {
    error = -error;
  }

  //String msg1 = String("Rotating to angle ");
  //String msgf = String(msg1 + doubleToS(angle));
  //location->say(msg1);
  float count = 0;
  int prevDir = 0;
  while (fabs(error) > ANGLE_RANGE && count <= TIMEOUT_COUNT)
  {
    //	msg1 = String("current error: ");
    //	String msg2 = String(" error range: ");
    //	String ns1 = doubleToS(error);
    //	String ns2 = doubleToS(ANGLE_RANGE);
    //	String msgf = String(msg1 + ns1 + msg2 + ns2 );
    /* location->say("error: ");
      location->say(error);
      location->say("\n");*/

    float w = fabs(K * error);

    //converts angular velocity to liner velocity
    float lspeed = w * WIDTH / 2;

    //convert linear speed to wheel w

    float wheelW = lspeed / WHEEL_RADIUS;

    float mspeed = wheelW;
    if (mspeed > 255) {
      mspeed = 255;
    }

    int dir = 0;
    if (error < 0) {
      dir = 1;
    }
    //go max speed
    //float mspeed = 255;

    if (dir != prevDir) {
      //stop the motors first if it is switching direction
      _leftm->setSpeed(0, 0);
      _rightm->setSpeed(0, 0);
      delay(500);
    }
    _leftm->setSpeed(mspeed, dir);
    // flips dir
    _rightm->setSpeed(mspeed, dir ^ 1);
    prevDir = dir;
    ca = location->getAngle();
    error = ca - angle;
    if (fabs(error) > PI) {
      error = -error;
    }

    delay(DELTA_TIME * 1000);
    // _leftm->setSpeed(0,0);
    // _rightm->setSpeed(0,0);

    /*location->say("bout to loop again, error:");
      location->say(error);
      location->say("\n");*/
    if (fabs(error) < TIMEOUT_ANGLE)
    {
      count = count + 1;
    }
  }
  //location->say("Done rotating to angle \n");
  //location->say("derror: ");
  //location->say(error);
  //location->say("\n");
  //	msg1 = String("current error: ");
  //	String msg2 = String(" error range: ");
  //	String ns1 = doubleToS(error);
  //	String ns2 = doubleToS(ANGLE_RANGE);
  //	String msgf = String(msg1 + ns1 + msg2 + ns2 );
  //	location->say(msgf);
  //	location->say(msgf);
  _leftm->setSpeed(0, 0);
  _rightm->setSpeed(0, 0);
}


void Navigator::navBoulders()
{
  // this->gotoWaypoint(0.500, 1.700);
  // this->rotateToAngle(0);
  //  location->say("Middle dist: ");
  //  location->say(_smiddle->getDistance());
  //  location->say(" cm\n");

  /*  while (_sright->getDistance() > 4 && _sleft->getDistance() > 4 && _smiddle->getDistance() > 6 && location->getX() < 1.900) {
      location->say(_smiddle->getDistance());
      location->say(" cm\n");
      _leftm->setSpeed(255, 0);
      _rightm->setSpeed(255, 0);
    }
  */
  float y = 1.100;
  boolean first = true;
  while (location->getX() < 1.300) {
    if (first) {
      first = false;
      this->gotoWaypoint(0.500, y);
    } else {
      this->gotoWaypoint(location->getX(), y);
    }
    //  if (location->getX() < 1.700) {
    //  this->backUp(0.5);

    rotateToAngle(0);
    float loc = location->getX();
    _leftm->setSpeed(255, 0);
    _rightm->setSpeed(255, 0);
    delay(3000);
    if (fabs(location->getX() - loc) < 0.070) {
      // location->say("stopped");
      y += 0.700;
      if (y > 1.700) {
        y = 1.700;
      }
      backUp(3000);
    }
    //this->gotoWaypoint(1.800, 1.000);
    //this->gotoWaypoint(1.800, 1.750);
  }
  //  }

  // some if statments and stuff
}

void Navigator::backUp(int timeToMove)
{
  _leftm->setSpeed(255, 1);
  _rightm->setSpeed(255, 1);
  delay(timeToMove);
  _leftm->setSpeed(0, 0);
  _rightm->setSpeed(0, 0);
}

/*void Navigator::navBoulders2()
{

  float y = 1.000;
  int attempt = 1;
  while (location->getX() < 1.400) {
    if (attempt = 1)
    {
      attempt = 2;
      this->gotoWaypoint(0.500, y);
    }
    else if (attempt == 2 || attempt == 3)
    {
      attempt = 3;
      this->gotoWaypoint(location->getX(), y);
    }
    //  if (location->getX() < 1.700) {
    //  this->backUp(0.5);

    rotateToAngle(0);
    float loc = location->getX();
    _leftm->setSpeed(255, 0);
    _rightm->setSpeed(255, 0);
    delay(2500);
    if (fabs(location->getX() - loc) < 0.070) {
      // location->say("stopped");
      y = (attempt == 2)? 1.700 : 0.300;
      backUp(3000);
    }
    //this->gotoWaypoint(1.800, 1.000);
    //this->gotoWaypoint(1.800, 1.750);
  }
  //  }

  // some if statments and stuff
}*/

