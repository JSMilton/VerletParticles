
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
    float updateConstraint = 1;
    if (screenPosition.x <= maxX && screenPosition.x >= minX && screenPosition.y <= maxY && screenPosition.y >= minY){
        accel = uMouseAcceleration * 1;
        updateConstraint = 0;
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
    
//    if (updateConstraint == 1){
//        vec3 attractor = vec3(0);
//        float restingDistance = 1.0;
//        
//        float diffX = attractor.x - vPosition.x;
//        float diffY = attractor.y - vPosition.y;
//        float d = sqrt(diffX * diffX + diffY * diffY);
//        // difference scalar
//        float difference = (restingDistance - d) / d;
//        
//        // translation for each PointMass. They'll be pushed 1/2 the required distance to match their resting distances.
//        float translateX = diffX * 0.5 * difference;
//        float translateY = diffY * 0.5 * difference;
//        
//        vVelocity.x += translateX;
//        vVelocity.y += translateY;
//    }
}