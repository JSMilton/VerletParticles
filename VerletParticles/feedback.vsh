
in vec3 aPosition;
in vec3 aVelocity;

out vec3 vPosition;
out vec3 vVelocity;

uniform float uDeltaTime;
uniform vec3 uMousePosition;
uniform vec3 uMouseAcceleration;
uniform float uAreaOfEffect;

void main()
{
    vec3 oldVel = aVelocity;
    vec3 accel = uMouseAcceleration * 0.1;
    vec3 newVel = aVelocity + accel * uDeltaTime;
    vVelocity = newVel;
    vVelocity *= 0.99;
    vPosition = aPosition + (oldVel + newVel) * 0.5 * uDeltaTime;
}

