
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
    vec4 screenMouse = vec4(uMousePosition, 1.0);
    screenMouse.xyz /= screenMouse.w;
    vec4 screenPosition = uModelViewProjectionMatrix * vec4(aPosition, 1.0);
    screenPosition.xyz /= screenPosition.w;
    screenMouse.y *= 3;
    screenMouse.x *= 4;
    
    float minX = uMousePosition.x - uAreaOfEffect;
    float maxX = uMousePosition.x + uAreaOfEffect;
    float minY = uMousePosition.y - uAreaOfEffect;
    float maxY = uMousePosition.y + uAreaOfEffect;
    
    float x = aPosition.x;
    float y = aPosition.y;
    float forceX = 0;
    float forceY = 0;
    float force = 0;
    float distSquared = pow(x - screenMouse.x, 2) + pow(y - screenMouse.y, 2);
    force = 0.5 / distSquared;
    forceX = (screenMouse.x - x) * force;
    forceY = (screenMouse.y - y) * force;
    
    
    vec3 accel = vec3(forceX, forceY, 0);
    vec3 oldVel = aVelocity;
    vec3 newVel = aVelocity + accel * uDeltaTime;
    
//    if (screenPosition.y >= 1.0 || screenPosition.y <= -1.0){
//        newVel.y *= -1;
//        oldVel.y *= -1;
//    }
//    
//    if (screenPosition.x >= 1.0 || screenPosition.x <= -1.0){
//        newVel.x *= -1;
//        oldVel.x *= -1;
//    }
    
    vPosition = aPosition + (oldVel + newVel) * 0.5 * uDeltaTime;
    vVelocity = newVel;
    vVelocity *= 0.99;
}