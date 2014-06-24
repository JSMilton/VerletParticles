
in vec3 aPosition;
in vec3 aPreviousPosition;
in vec3 aAcceleration;

out vec3 vPosition;
out vec3 vPreviousPosition;
out vec3 vAcceleration;

//uniform float uDeltaTime;

void main()
{
    vPreviousPosition = aPosition;
    vPosition = aPosition + 0.1;
    vAcceleration = vec3(0);
}
