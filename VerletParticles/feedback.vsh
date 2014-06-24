
in vec3 aPosition;
in vec3 aVelocity;

out vec3 vPosition;
out vec3 vVelocity;

uniform float uDeltaTime;
uniform vec3 uMousePosition;
uniform vec3 uMouseAcceleration;
uniform float uAreaOfEffect;

uniform mat4 uModelViewProjectionMatrix;

void main()
{
    vec3 oldVel = aVelocity;
    vec3 accel = uMouseAcceleration * 0.1;
    vec3 newVel = aVelocity + accel * uDeltaTime;
    vec4 screenPosition = uModelViewProjectionMatrix * vec4(aPosition, 1.0);
    if (screenPosition.y > 5.0 || screenPosition.y < -5.0)
        newVel *= -1;
    vVelocity = newVel;
    vVelocity *= 0.99;
    vPosition = aPosition + (oldVel + newVel) * 0.5 * uDeltaTime;
}

