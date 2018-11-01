#include "DefenderRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CartesianCoord.h"

DefenderRole::DefenderRole(SpellBook *spellBook) : InnerModule(spellBook)
{

    onBall = false;
    onPosition = false;
}
DefenderRole::~DefenderRole()
{
}
void DefenderRole::Tick(float ellapsedTime, const SensorValues &sensor)
{
    CartesianCoord coord;
    RelativeCoord rr;
    if ((spellBook->strategy.GameState == GC::READY || spellBook->strategy.GameState == GC::PLAYING) &&
        !onPosition)
    {


        if (spellBook->perception.vision.localization.Enabled)
        {
            spellBook->strategy.WalkForward = true;
            spellBook->strategy.TargetX = 0.0f;
            spellBook->strategy.TargetY = 0.0f;
            spellBook->strategy.TargetTheta = 0;

            CartesianCoord current(spellBook->perception.vision.localization.X, spellBook->perception.vision.localization.Y);
            CartesianCoord desired(spellBook->strategy.TargetX, spellBook->strategy.TargetY);
            if (current.distance(desired) < 0.2f)
            {
                spellBook->strategy.WalkForward = false;
                onPosition = true;
            }
        }
        else
        {
            onPosition = true;
        }
    }
    if (spellBook->strategy.GameState == GC::PLAYING)
    {
        spellBook->motion.KickRight = false;
        spellBook->motion.Vth = 0;
        spellBook->motion.Vx = 0;
        spellBook->motion.Vy = 0;
        if (spellBook->perception.vision.ball.BallDetected)
        {
            cout << "Valor atual do GetX: " << coord.getX() << endl;
            rr.fromPixel(spellBook->perception.vision.ball.ImageX, spellBook->perception.vision.ball.ImageY, sensor.joints.angles[Joints::HeadYaw], -sensor.joints.angles[Joints::HeadPitch]);
            rr.toCartesian(coord, sensor.joints.angles[Joints::HeadYaw], sensor.joints.angles[Joints::HeadPitch]);
            cout << "X: " << coord.getX() << endl;
            cout << "Y: " << coord.getY() << endl;
            if (rr.getDistance() > 0.8f)
            {
                if (rr.getYaw() > Deg2Rad(5) || rr.getYaw() < Deg2Rad(-5))
                {
                    spellBook->motion.Vth = min(rr.getYaw() * rr.getDistance(), Deg2Rad(0.5f));
                    spellBook->motion.Vx = abs(min(coord.getX(), 0.1f));
                    spellBook->motion.Vy = (coord.getY(), 0.05f);
                }
                else
                {
                    spellBook->motion.Vth = 0;
                    spellBook->motion.Vx = (coord.getX() * 0.1);
                    spellBook->motion.Vy = 0;
                }
            }
            else if(rr.getDistance() > 0.5f && rr.getDistance() < 0.8f)
            {
                if (rr.getYaw() > Deg2Rad(8) || rr.getYaw() < Deg2Rad(-8))
                {
                    spellBook->motion.Vth = min(rr.getYaw() * rr.getDistance(), Deg2Rad(0.5f));
                    spellBook->motion.Vx = abs(min(coord.getX(), 0.15f));
                    spellBook->motion.Vy = coord.getY()*0.05f;
                }
                else
                {
                    spellBook->motion.Vth = 0;
                    spellBook->motion.Vx = abs((coord.getX() * 0.03));
                    spellBook->motion.Vy = 0;
                }
            }
            else{
                spellBook->motion.Vx = 0.0f;
            }
            /*if (rr.getDistance() < 0.4f)
            {
                cout << "ENTROU NA MERDA DO IF E A DISTANCIA É: " << rr.getDistance();
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0;
                spellBook->motion.Vy = 0;
                spellBook->motion.KickRight = true;
                
            } */
        }
        else
        {
            cout << "se perdeu: "<< endl;
            spellBook->motion.Vx = 0.01f;
            spellBook->motion.Vy = 0.15f;
        }
    }
}