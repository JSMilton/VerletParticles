
in vec3 aPosition;
in vec3 aPreviousPosition;
in vec3 aAcceleration;

out vec3 vPosition;
out vec3 vPreviousPosition;
out vec3 vAcceleration;

uniform float uDeltaTime;
uniform vec3 uMousePosition;
uniform vec3 uMousePreviousPosition;
uniform float uAreaOfEffect;

void main()
{
    vPreviousPosition = aPosition;
    vPosition = aPosition + 0.001;
    vAcceleration = vec3(0);
}
