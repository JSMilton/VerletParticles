
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
    vec4 screenMouse = uModelViewProjectionMatrix * vec4(uMousePosition, 1.0);
    screenMouse.xyz /= screenMouse.w;
    vec4 screenPosition = uModelViewProjectionMatrix * vec4(aPosition, 1.0);
    screenPosition.xyz /= screenPosition.w;
    
    float minX = uMousePosition.x - uAreaOfEffect;
    float maxX = uMousePosition.x + uAreaOfEffect;
    float minY = uMousePosition.y - uAreaOfEffect;
    float maxY = uMousePosition.y + uAreaOfEffect;
    
    vec3 accel = vec3(0);
    
    if (screenPosition.x <= maxX && screenPosition.x >= minX && screenPosition.y <= maxY && screenPosition.y >= minY){
        accel = uMouseAcceleration;
    }
    
    vec3 oldVel = aVelocity;
    vec3 newVel = aVelocity + accel * uDeltaTime;
    
    if (screenPosition.y >= 1.0 || screenPosition.y <= -1.0){
        newVel.y *= -1;
        oldVel.y *= -1;
    }
    
    if (screenPosition.x >= 1.0 || screenPosition.x <= -1.0){
        newVel.x *= -1;
        oldVel.x *= -1;
    }
    
    vPosition = aPosition + (oldVel + newVel) * 0.5 * uDeltaTime;
    vVelocity = newVel;
    vVelocity *= 0.99;
}