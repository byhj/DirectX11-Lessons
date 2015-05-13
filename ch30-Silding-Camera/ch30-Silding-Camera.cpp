#include "d3dApp.h"

//Vertex Structure and Vertex Layout (Input Layout)//
struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
};
struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

Light light;

struct cbPerFrame
{
	Light  light;
};

cbPerFrame  cbPerLight;
cbPerObject cbPerObj;

/************************************New Stuff****************************************************/
// This is the scale of our scene, which we will use to find "a very small distance"
// If this is set too small, you will notice the camera "stick" to the geometry once in a while
// Just play with it and use whatever works best for your application
const float unitsPerMeter = 100.0f;

// The gravity's velocity vector
XMVECTOR gravity = XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f);

// Polygon Soup
std::vector<XMFLOAT3> collidableGeometryPositions;
std::vector<DWORD> collidableGeometryIndices;

struct CollisionPacket{
	// Information about ellipsoid (in world space)
	XMVECTOR ellipsoidSpace;
	XMVECTOR w_Position;
	XMVECTOR w_Velocity;

	// Information about ellipsoid (in ellipsoid space)
	XMVECTOR e_Position;
	XMVECTOR e_Velocity;
	XMVECTOR e_normalizedVelocity;

	// Collision Information
	bool foundCollision;
	float nearestDistance;
	XMVECTOR intersectionPoint;
	int collisionRecursionDepth;
};

// Collision Detection and Response Function Prototypes
XMVECTOR CollisionSlide(CollisionPacket& cP,			// Pointer to a CollisionPacket object (expects ellipsoidSpace, w_Position and w_Velocity to be filled)
						std::vector<XMFLOAT3>& vertPos,						// An array holding the polygon soup vertex positions
						std::vector<DWORD>& indices);						// An array holding the polygon soup indices (triangles)

XMVECTOR CollideWithWorld(CollisionPacket& cP,			// Same arguments as the above function
						  std::vector<XMFLOAT3>& vertPos,
						  std::vector<DWORD>& indices);

bool SphereCollidingWithTriangle(CollisionPacket& cP,	// Pointer to a CollisionPacket object	
								 XMVECTOR &p0,										// First vertex position of triangle
								 XMVECTOR &p1,										// Second vertex position of triangle
								 XMVECTOR &p2,										// Third vertex position of triangle 
								 XMVECTOR &triNormal);								// Triangle's Normal

// Checks if a point (inside the triangle's plane) is inside the triangle
bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1,const XMVECTOR& triV2, const XMVECTOR& triV3);

// Solves the quadratic eqation, and returns the lowest root if equation is solvable, returns false if not solvable
bool getLowestRoot(float a, float b, float c, float maxR, float* root);
/*************************************************************************************************/

int NumFaces = 0;
int NumVertices = 0;

struct HeightMapInfo{		// Heightmap structure
	int terrainWidth;		// Width of heightmap
	int terrainHeight;		// Height (Length) of heightmap
	XMFLOAT3 *heightMap;	// Array to store terrain's vertex positions
};

bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationz;

XMMATRIX WVP;
XMMATRIX camView;
XMMATRIX camProjection;

XMMATRIX d2dWorld;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

///////////////**************new**************////////////////////
XMVECTOR DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
///////////////**************new**************////////////////////

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

const int Width = 1000;
const int Height = 800;

//Fps Data and Function
double countsPerSecond = 0.0;
__int64 CounterStart = 0;
int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;
void StartTimer();
double GetTime();
double GetFrameTime();

class TextureApp : public D3D11App
{
public:
	TextureApp(HINSTANCE hInstance);
	~TextureApp();

	bool InitScene();
	bool InitBuffer();
	bool InitShader();
	bool InitD3D();
	bool InitStatus();
	bool InitTexture();
	int	 Run();
	void UpdateScene(double time);
	void RenderScene();
	void RenderText(std::wstring text, int inInt);
	void InitD2DScreenTexture();
	void UpdateCamera();
	bool InitDirectInput(HINSTANCE hInstance);
	void DetectInput(double time);

private:
	ID3D11Buffer* squareIndexBuffer;
	ID3D11Buffer* squareVertBuffer;

	ID3D11VertexShader    *pVS;
	ID3D11PixelShader     *pPS;
	ID3D10Blob            *pVS_Buffer;
	ID3D10Blob            *pPS_Buffer;
	ID3D11InputLayout     *pInputLayout;
	ID3D11Buffer          *cbPerObjectBuffer;
	ID3D11Buffer          *cbPerFrameBuffer;
	ID3D11PixelShader     *pD2D_PS;
	ID3D10Blob            *pD2D_PS_Buffer;
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;

	HRESULT hr;
};

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//init the window
	TextureApp app(hInstance);
	if ( !app.InitScene() )
		return 0;

	return  app.Run();
}

int TextureApp::Run()
{
	MSG msg = {0};

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{	
			frameCount++;
			if(GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}	
			frameTime = GetFrameTime();
			DetectInput(frameTime);
			UpdateScene(frameTime);

			RenderScene();
		}
	}

	return (int)msg.wParam;
}

TextureApp::TextureApp(HINSTANCE hInstance)
	:D3D11App(hInstance)
{
}

TextureApp::~TextureApp()
{
}

/************************************New Stuff****************************************************/
// This is the function we will call when we want to find if an ellipsoid will collide with
// the world (polygon soup) while traveling along it's velocity vector, and also impliment
// gravity by doing the collision detection and response algorithm with the gravity's
// velocity vector. It's kind of like the collision detection and response package
XMVECTOR CollisionSlide(CollisionPacket& cP,
						std::vector<XMFLOAT3>& vertPos,
						std::vector<DWORD>& indices)
{
	// Transform velocity vector to the ellipsoid space (e_ denotes ellipsoid space)
	cP.e_Velocity = cP.w_Velocity/cP.ellipsoidSpace;

	// Transform position vector to the ellipsoid space
	cP.e_Position = cP.w_Position/cP.ellipsoidSpace;

	// Now we check for a collision with our world, this function will
	// call itself 5 times at most, or until the velocity vector is
	// used up (very small (near zero to zero length))
	cP.collisionRecursionDepth = 0;
	XMVECTOR finalPosition = CollideWithWorld(cP, vertPos, indices);

	// Add gravity pull:
	// This is simply adding a new velocity vector in the downward
	// direction (defined globaly) to pull the ellipsoid down, then doing the
	// collision check against all the geometry again. The way it is now, the
	// ellipsoid will "slide" down even the most slightest slope. Consider this
	// an exercise: only impliment gravity when standing on a very steep slope,
	// or if you are not standing on anything at all (free fall)

	// To remove gravity uncomment from here .....

	cP.e_Velocity = gravity / cP.ellipsoidSpace;	// We defined gravity in world space, so now we have
	// to convert it to ellipsoid space
	cP.e_Position = finalPosition;
	cP.collisionRecursionDepth = 0;
	finalPosition = CollideWithWorld(cP, vertPos, indices);

	// ... to here

	// Convert our final position from ellipsoid space to world space
	finalPosition = finalPosition * cP.ellipsoidSpace;

	// Return our final position!
	return finalPosition;
}

// This function impliments the collision detection and collision response
XMVECTOR CollideWithWorld(CollisionPacket& cP,
						  std::vector<XMFLOAT3>& vertPos,
						  std::vector<DWORD>& indices)
{
	// These are based off the unitsPerMeter from above
	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;	// This is used to keep the sphere from actually "touching" 
	// the triangle, as that would cause problems since
	// each loop it would ALWAYS find a collision instead
	// of just sliding along the triangle

	// This will stop us from entering an infinite loop, or a very long loop. For example, there are times when the sphere
	// might actually be pushed slightly into the triangles center, where the recursion will keep repeating and finding a collision
	// even though the velocity vector does not change (I had serious problems with this part for a couple days... I couldn't
	// figure out why the ellipsoid would LAUNCH at certain times, but if i set the ellipsoid space to (1,1,1) (a sphere), it would
	// act normal. Stupid me made a mistake and was returning w_Position here instead of e_Position, so that the world space position
	// was being multiplied by the ellipsoid space and "launching" it whenever it accidently got pushed into a triangle)
	if (cP.collisionRecursionDepth > 5)
		return cP.e_Position;

	// Normalize velocity vector
	cP.e_normalizedVelocity = XMVector3Normalize(cP.e_Velocity);	

	// Initialize collision packet stuff
	cP.foundCollision = false;
	cP.nearestDistance = 0.0f;

	// Loop through each triangle in mesh and check for a collision
	for(int triCounter = 0; triCounter < indices.size() / 3; triCounter++)
	{
		// Get triangle
		XMVECTOR p0, p1, p2, tempVec;
		p0 = XMLoadFloat3(&vertPos[indices[3*triCounter]]);
		p1 = XMLoadFloat3(&vertPos[indices[3*triCounter+1]]);
		p2 = XMLoadFloat3(&vertPos[indices[3*triCounter+2]]);

		// Put triangle into ellipsoid space
		p0 = p0/cP.ellipsoidSpace;
		p1 = p1/cP.ellipsoidSpace;
		p2 = p2/cP.ellipsoidSpace;

		// Calculate the normal for this triangle
		XMVECTOR triNormal;
		triNormal = XMVector3Normalize(XMVector3Cross((p1 - p0),(p2 - p0)));

		// Now we check to see if the sphere is colliding with the current triangle
		SphereCollidingWithTriangle(cP, p0, p1, p2, triNormal);
	}

	// If there was no collision, return the position + velocity
	if (cP.foundCollision == false) {
		return cP.e_Position + cP.e_Velocity;
	}

	// If we've made it here, a collision occured
	// destinationPoint is where the sphere would travel if there was
	// no collisions, however, at this point, there has a been a collision
	// detected. We will use this vector to find the new "sliding" vector
	// based off the plane created from the sphere and collision point
	XMVECTOR destinationPoint = cP.e_Position + cP.e_Velocity;

	XMVECTOR newPosition = cP.e_Position;	// Just initialize newPosition

	// If the position is further than "veryCloseDistance" from the point
	// of collision, we will move the sphere along the velocity path until
	// it "almost" touches the triangle, or point of collision. We do this so
	// that the next recursion (if there is one) does not detect a collision
	// with the triangle we just collided with. We don't need to find a collision
	// with the triangle we just collided with because we will be moving parallel
	// to it now, and if we were finding the collision with it every recursion
	// (since it's the closest triangle we would collide with), we would 
	// finish our 5 recursions (checked above) without ever moving after
	// touching a triangle, because before the triangle has a chance to move
	// down the new velocity path (even though it's about parallel with the triangle)
	// it would find the collision with the triangle, and simply recompute the same
	// velocity vector it computed the first time. This would happen because of
	// floating point innacuracy. theoretically, we would not have to worry about this
	// because after the new velocity vector is created, it SHOULD be perfectly parallel
	// to the triangle, and we detect that in our code and basically skip triangles
	// who are perfectly parallel with the velocity vector. But like i said, because
	// of innacuracy, the new velocity vector might be VERY SLIGHTLY pointed down towards
	// the triangles plane, which would make us waste a recursion just to recompute the same
	// velocity vector. Basically, the whole sliding thing works without this, but it's a lot
	// more "choppy" and "sticky", where you get stuck in random places.
	if (cP.nearestDistance >= veryCloseDistance)
	{
		// Move the new position down velocity vector to ALMOST touch the collision point
		XMVECTOR V = cP.e_Velocity;
		V = XMVector3Normalize(V);
		V = V * (cP.nearestDistance - veryCloseDistance);
		newPosition = cP.e_Position + V;

		// Adjust polygon intersection point (so sliding
		// plane will be unaffected by the fact that we
		// move slightly less than collision tells us)
		V = XMVector3Normalize(V);
		cP.intersectionPoint -= veryCloseDistance * V;
	}

	// This is our sliding plane (point in the plane and plane normal)
	XMVECTOR slidePlaneOrigin = cP.intersectionPoint;
	XMVECTOR slidePlaneNormal =	newPosition - cP.intersectionPoint;
	slidePlaneNormal = XMVector3Normalize(slidePlaneNormal);

	// We will use the sliding plane to compute our new "destination" point
	// and new velocity vector. To do this, we will need to solve another quadratic
	// equation (Ax + By + Cz + D = 0), where D is what we call the plane constant,
	// which we use to find the distance between the sliding plane and our original
	// destination point (original as up until now, since it's likely that this is
	// not the first recursion of this function, and the original original destination
	// has been changed up until now).
	// First the point in the plane
	float x = XMVectorGetX(slidePlaneOrigin);
	float y = XMVectorGetY(slidePlaneOrigin);
	float z = XMVectorGetZ(slidePlaneOrigin);

	// Next the planes normal
	float A = XMVectorGetX(slidePlaneNormal);
	float B = XMVectorGetY(slidePlaneNormal);
	float C = XMVectorGetZ(slidePlaneNormal);
	float D = -((A*x) + (B*y) + (C*z));

	// To keep the variable names clear, we will rename D to planeConstant
	float planeConstant = D;

	// Get the distance between sliding plane and destination point
	float signedDistFromDestPointToSlidingPlane = XMVectorGetX(XMVector3Dot(destinationPoint, slidePlaneNormal)) + planeConstant;

	// Now we calculate the new destination point. To get the new destination point, we will subtract
	// the distance from the plane to the original destination point (down the planes normal) from the
	// original destination point. It's easier to picture this in your head than explain, so let me try
	// to give you a very simple picture. Pretend you are this equation, standing on the plane, where UP
	// (your head) is pointing the same direction as the plane's normal. directly below you is the "destination"
	// point of the sphere. Your job as this equation is to "pull" the destination point up (towards the planes
	// normal) until it is resting "in" the plane. If you can picture this the way i'm trying to get you to, you
	// can see that the new velocity vector (from the point of collision between sphere and plane) to the new
	// destination is "shorter" and parallel to the plane, so that now when the sphere follows this new velocity
	// vector, it will be traveling parallel (sliding) across the triangle, at the same time, it does not travel
	// as far as it would have if there was no collision. This is exactly what we want, because when you think about
	// it, we do not run up mountains as fast as we run on flat ground, and if we run straight into a wall in our
	// game, we will just stop moving, or if we run ALMOST straight into the wall, we will not go cruising sideways,
	// but instead slowly move to either side. In my lesson on braynzarsoft.net, This is explained in pictures
	XMVECTOR newDestinationPoint = destinationPoint - signedDistFromDestPointToSlidingPlane * slidePlaneNormal;

	// I believe this line was covered briefly in the above explanation
	XMVECTOR newVelocityVector = newDestinationPoint - cP.intersectionPoint;

	// After this check, we will recurse. This check makes sure that we have not
	// come to the end of our velocity vector (or very close to it, because if the velocity
	// vector is very small, there is no reason to lose performance by doing an extra recurse
	// when we won't even notice the distance "thrown away" by this check anyway) before
	// we recurse
	if (XMVectorGetX(XMVector3Length(newVelocityVector)) < veryCloseDistance) {
		return newPosition;
	}

	// We are going to recurse now since a collision was found and the velocity
	// changed directions. we need to check if the new velocity vector will
	// cause the sphere to collide with other geometry.
	cP.collisionRecursionDepth++;
	cP.e_Position = newPosition;
	cP.e_Velocity = newVelocityVector;
	return CollideWithWorld(cP, vertPos, indices);
}

// This function checks if the swept sphere collides with a single triangle
bool SphereCollidingWithTriangle(CollisionPacket& cP, XMVECTOR &p0, XMVECTOR &p1, XMVECTOR &p2, XMVECTOR &triNormal)
{
	// This function assumes p0, p1, p2, and the triangle normal are in ellipsoid space
	// and that e_Position e_Velocity, and e_normalizedVelocity are defined in ellipsoid space
	// In other words, this function checks for a collision between a SPHERE and a triangle,
	// not an ellipsoid and a triangle. Because of this, the results from this function
	// (specifically cP.nearestDistance and cP.intersectionPoint) are in ellipsoid space

	// Check to see if triangle is facing velocity vector
	// We will not triangle facing away from the velocity vector to speed this up
	// since we assume that we will never run into the back face of triangles
	float facing = XMVectorGetX(XMVector3Dot(triNormal, cP.e_normalizedVelocity));
	if(facing <= 0)
	{ 
		// Create these because cP.e_Velocity and cP.e_Position add slightly to the difficulty
		// of reading the equations
		XMVECTOR velocity = cP.e_Velocity;
		XMVECTOR position = cP.e_Position;

		// t0 and t1 hold the time it takes along the velocity vector that the sphere (called a swept sphere)
		// will "collide" (resting on or touching), once on the front side of the triangle (t0), and once on the
		// backside after it goes "through" the triangle (t1) (or vertex or edge).
		float t0, t1;

		// If sphere is in the plane, it will not intersect with the center of the triangle
		// but instead possibly intersect with one of the vertices or edges first
		bool sphereInPlane = false;

		// Find the plane equation in which the triangle lies in (Ax + By + Cz + D = 0)
		// A, B, and C are the planes normal, x, y, and z respectively
		// We can find D (a.k.a the plane constant) using some simple algebra, which we will do below
		// x, y, and z in the equation defines a point in the plane. Any point in the plane
		// will do, so we will just use p0

		// First the point in the plane
		float x = XMVectorGetX(p0);
		float y = XMVectorGetY(p0);
		float z = XMVectorGetZ(p0);

		// Next the planes normal
		float A = XMVectorGetX(triNormal);
		float B = XMVectorGetY(triNormal);
		float C = XMVectorGetZ(triNormal);

		// Lets solve for D
		// step 1: 0 = Ax + By + Cz + D
		// step 2: subtract D from both sides
		//			-D = Ax + By + Cz
		// setp 3: multiply both sides by -1
		//			-D*-1 = -1 * (Ax + By + Cz)
		// final answer: D = -(Ax + By + Cz)
		float D = -((A*x) + (B*y) + (C*z));

		// To keep the variable names clear, we will rename D to planeConstant
		float planeConstant = D;

		// Get the signed distance from the cameras position (or object if you are using an object)
		// We can get the signed distance between a point and plane with the equation:
		// SignedDistance = PlaneNormal * Point + PlaneConstant 

		// I've mentioned this before, but i'll do it again. When using xna math library vector function
		// that return a scalar value (like a float) such as "XMVector3Dot", an XMVECTOR is returned, with
		// all elements (x,y,z,w) containing that scalar value. We need to extract one, and any will do since
		// they are all the same, so we extract the x component using "XMVectorGetX"
		float signedDistFromPositionToTriPlane = XMVectorGetX(XMVector3Dot(position, triNormal)) + planeConstant;

		// This will be used a couple times below, so we'll just calculate and store it now
		float planeNormalDotVelocity = XMVectorGetX(XMVector3Dot(triNormal, velocity));

		/////////////////////////////////////Sphere Plane Collision Test////////////////////////////////////////////
		// Check to see if the velocity vector is parallel with the plane
		if (planeNormalDotVelocity == 0.0f) 
		{
			if (fabs(signedDistFromPositionToTriPlane) >= 1.0f) 
			{
				// sphere not in plane, and velocity is
				// parallel to plane, no collision possible
				return false;
			}
			else 
			{
				// sphere is in the plane, so we will now only test for a collision
				// with the triangle's vertices and edges
				// Set sphereInPlane to true so we do not do the operation
				// which will divide by zero if the velocity and plane are parallel
				sphereInPlane = true;
			}
		}
		else 
		{
			// We know the velocity vector at some point intersects with the plane, we just
			// need to find how far down the velocity vector the sphere will "touch" or rest
			// on the plane. t0 is when it first touches the plane (front side of sphere touches)
			// and t1 is when the back side of the sphere touches.

			// To find when (the time or how far down the velocity vector) the "velocity vector" itself
			//intersects with the plane, we use the equation: (* stands for a dot product)
			// t = (PlaneNormal * Point + PlaneConstant) / (PlaneNormal * Velocity);
			// We have already calculated both sides of the divide sign "/", so:
			// t = signedDistance / normalDotVelocity;

			// Now remember we are working with a unit sphere (since everything has been moved from
			// the usual space to our ellipsoid space). The unit sphere means that the distance from
			// the center of the sphere to ANYWHERE on it's surface is "1". We are not interested in
			// finding when the actual velocity vector intersects with the plane, but instead when
			// the surface of the sphere "touches" the surface of the plane. We know that the distance
			// from the center of the sphere is "1", so all we have to do to find when the sphere touches
			// the plane is subtract and subtract 1 from the signed distance to get when both sides of the
			// sphere touch the plane (t0, and t1)
			t0 = ( 1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;
			t1 = (-1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;

			// We will make sure that t0 is smaller than t1, which means that t0 is when the sphere FIRST
			// touches the planes surface
			if(t0 > t1)
			{
				float temp = t0;
				t0 = t1;
				t1 = temp;
			}

			// If the swept sphere touches the plane outside of the 0 to 1 "timeframe", we know that
			// the sphere is not going to intersect with the plane (and of course triangle) this frame
			if (t0 > 1.0f || t1 < 0.0f) 
			{
				return false;
			}

			// If t0 is smaller than 0 then we will make it 0
			// and if t1 is greater than 1 we will make it 1
			if (t0 < 0.0) t0 = 0.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		////////////////////////////////Sphere-(Inside Triangle) Collision Test///////////////////////////////////////
		// If we've made it this far, we know that the sphere will intersect with the triangles plane
		// This frame, so now we will check to see if the collision happened INSIDE the triangle
		XMVECTOR collisionPoint;		// Point on plane where collision occured
		bool collidingWithTri = false;	// This is set so we know if there was a collision with the CURRENT triangle
		float t = 1.0;					// Time 

		// If the sphere is not IN the triangles plane, we continue the sphere to inside of triangle test
		if (!sphereInPlane) 
		{
			// We get the point on the triangles plane where the sphere "touches" the plane
			// using the equation: planeIntersectionPoint = (Position - Normal) + t0 * Velocity
			// Where t0 is the distance down the velocity vector that the sphere first makes
			// contact with the plane
			XMVECTOR planeIntersectionPoint = (position + t0 * velocity - triNormal);

			// Now we call the function that checks if a point on a triangle's plane is inside the triangle
			if (checkPointInTriangle(planeIntersectionPoint,p0,p1,p2))
			{
				// If the point on the plane IS inside the triangle, we know that the sphere is colliding
				// with the triangle now, so we set collidingWithTri to true so we don't do all the extra
				// calculations on the triangle. We set t to t0, which is the time (or distance) down
				// the velocity vector that the sphere first makes contact, then we set the point of 
				// collision, which will be used later for our collision response
				collidingWithTri = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}

		/////////////////////////////////////Sphere-Vertex Collision Test//////////////////////////////////////////////
		// If the sphere is not colliding with the triangles INSIDE, we check to see if it will collide with one of
		// the vertices of the triangle using the sweep test we did above, but this time check for each vertex instead
		// of the triangles plane
		if (collidingWithTri == false) 
		{
			// We will be working with the quadratic function "At^2 + Bt + C = 0" to find when (t) the "swept sphere"s center
			// is 1 unit (spheres radius) away from the vertex position. Remember the swept spheres position is actually a line defined
			// by the spheres position and velocity. t represents it's position along the velocity vector.
			// a = sphereVelocityLength * sphereVelocityLength
			// b = 2(sphereVelocity . (spherePosition - vertexPosition))	// . denotes dot product
			// c = (vertexPosition - spherePosition)^2 - 1
			// This equation allows for two solutions. One is when the sphere "first" touches the vertex, and the other is when
			// the "other" side of the sphere touches the vertex on it's way past the vertex. We need the first "touch"
			float a, b, c; // Equation Parameters

			// We can use the squared velocities length below when checking for collisions with the edges of the triangles
			// to, so to keep things clear, we won't set a directly
			float velocityLengthSquared = XMVectorGetX(XMVector3Length(velocity));
			velocityLengthSquared *= velocityLengthSquared;

			// We'll start by setting 'a', since all 3 point equations use this 'a'
			a = velocityLengthSquared;

			// This is a temporary variable to hold the distance down the velocity vector that
			// the sphere will touch the vertex.
			float newT;

			// P0 - Collision test with sphere and p0
			b = 2.0f * ( XMVectorGetX( XMVector3Dot( velocity, position - p0 )));
			c = XMVectorGetX(XMVector3Length((p0 - position)));
			c = (c*c) - 1.0f;
			if (getLowestRoot(a,b,c, t, &newT)) {	// Check if the equation can be solved
				// If the equation was solved, we can set a couple things. First we set t (distance
				// down velocity vector the sphere first collides with vertex) to the temporary newT,
				// Then we set collidingWithTri to be true so we know there was for sure a collision
				// with the triangle, then we set the exact point the sphere collides with the triangle,
				// which is the position of the vertex it collides with
				t = newT;
				collidingWithTri = true;
				collisionPoint = p0;
			}

			// P1 - Collision test with sphere and p1
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - p1)));
			c = XMVectorGetX(XMVector3Length((p1 - position)));
			c = (c*c) - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = p1;
			}

			// P2 - Collision test with sphere and p2
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - p2)));
			c = XMVectorGetX(XMVector3Length((p2 - position)));
			c = (c*c) - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = p2;
			}

			//////////////////////////////////////////////Sphere-Edge Collision Test//////////////////////////////////////////////
			// Even though there might have been a collision with a vertex, we will still check for a collision with an edge of the
			// triangle in case an edge was hit before the vertex. Again we will solve a quadratic equation to find where (and if)
			// the swept sphere's position is 1 unit away from the edge of the triangle. The equation parameters this time are a 
			// bit more complex: (still "Ax^2 + Bx + C = 0")
			// a = edgeLength^2 * -velocityLength^2 + (edge . velocity)^2
			// b = edgeLength^2 * 2(velocity . spherePositionToVertex) - 2((edge . velocity)(edge . spherePositionToVertex))
			// c =  edgeLength^2 * (1 - spherePositionToVertexLength^2) + (edge . spherePositionToVertex)^2
			// . denotes dot product

			// Edge (p0, p1):
			XMVECTOR edge = p1 - p0;
			XMVECTOR spherePositionToVertex = p0 - position;
			float edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared *= edgeLengthSquared;
			float edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
			float edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			float spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			// Equation parameters
			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			// We start by finding if the swept sphere collides with the edges "infinite line"
			if (getLowestRoot(a,b,c, t, &newT)) {
				// Now we check to see if the collision happened between the two vertices that make up this edge
				// We can calculate where on the line the collision happens by doing this:
				// f = (edge . velocity)newT - (edge . spherePositionToVertex) / edgeLength^2
				// if f is between 0 and 1, then we know the collision happened between p0 and p1
				// If the collision happened at p0, the f = 0, if the collision happened at p1 then f = 1
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					// If the collision with the edge happened, we set the results
					t = newT;
					collidingWithTri = true;
					collisionPoint = p0 + f * edge;
				}
			}

			// Edge (p1, p2):
			edge = p2 - p1;
			spherePositionToVertex = p1 - position;
			edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
			edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, cP.e_Velocity));
			edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a,b,c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					t = newT;
					collidingWithTri = true;
					collisionPoint = p1 + f * edge;
				}
			}

			// Edge (p2, p0):
			edge = p0 - p2;
			spherePositionToVertex = p2 - position;
			edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
			edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
			edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a,b,c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					t = newT;
					collidingWithTri = true;
					collisionPoint = p2 + f * edge;
				}
			}
		}

		// If we have found a collision, we will set the results of the collision here
		if (collidingWithTri == true) 
		{
			// We find the distance to the collision using the time variable (t) times the length of the velocity vector
			float distToCollision = t * XMVectorGetX(XMVector3Length(velocity));

			// Now we check if this is the first triangle that has been collided with OR it is 
			// the closest triangle yet that was collided with
			if (cP.foundCollision == false || distToCollision < cP.nearestDistance) {

				// Collision response information (used for "sliding")
				cP.nearestDistance = distToCollision;
				cP.intersectionPoint = collisionPoint;

				// Make sure this is set to true if we've made it this far
				cP.foundCollision = true;
				return true;
			}			
		}
	}
	return false;
}

// These are the "helper" functions
// This function is found in my lesson on picking in direct3d 11
// This function finds if a point (in the triangle plane) is INSIDE the triangle
bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1,const XMVECTOR& triV2, const XMVECTOR& triV3)
{
	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
		}
	}
	return false;
}

// This function solves the quadratic eqation "At^2 + Bt + C = 0" and is found in Kasper Fauerby's paper on collision detection and response
bool getLowestRoot(float a, float b, float c, float maxR, float* root) 
{
	// Check if a solution exists
	float determinant = b*b - 4.0f*a*c;
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;
	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but let��s disregard that slight optimization)
	float sqrtD = sqrt(determinant);
	float r1 = (-b - sqrtD) / (2*a);
	float r2 = (-b + sqrtD) / (2*a);
	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	// Get lowest root:
	if (r1 > 0 && r1 < maxR) {
		*root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) {
		*root = r2;
		return true;
	}

	// No (valid) solutions
	return false;
}
/*************************************************************************************************/
bool HeightMapLoad(char* filename, HeightMapInfo &hminfo)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	// Open the file
	filePtr = fopen(filename,"rb");
	if (filePtr == NULL)
		return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k=0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for(int j=0; j< hminfo.terrainHeight; j++)
	{
		for(int i=0; i< hminfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = ( hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;

			k+=3;
		}
	}

	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}
void TextureApp::InitD2DScreenTexture()
{
	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	};


	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth      = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &pD2DIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth      = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &pD2DVertBuffer);


	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	pD3D11Device->CreateShaderResourceView(pSharedTex11, NULL, &pD2DTexture);

}
bool TextureApp::InitDirectInput(HINSTANCE hInstance)
{
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL); 

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	return true;
}

///////////////**************new**************////////////////////
void TextureApp::UpdateCamera()
{
		camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

	/************************************New Stuff****************************************************/
	// First-Person Camera
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);
	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	/*
	// Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);
	*/

	CollisionPacket cameraCP;
	cameraCP.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	cameraCP.w_Position = camPosition;
	cameraCP.w_Velocity = (moveLeftRight*camRight)+(moveBackForward*camForward);

	camPosition = CollisionSlide(cameraCP,
		collidableGeometryPositions,
		collidableGeometryIndices);

	/*camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;*/
	/*************************************************************************************************/

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;	

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
}

void TextureApp::DetectInput(double time)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if(keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * time;

	if(keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if(keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if(keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if(keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}


	UpdateCamera();

	return;
}
///////////////**************new**************////////////////////

bool TextureApp::InitScene()
{
	if(!D3D11App::InitWindow())
		return false;

	if(!D3D11App::InitD3D())
		return false;

	InitD2DScreenTexture();

	if(!InitShader())
		return false;

	if(!InitBuffer())
		return false;

	if(!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitStatus())
		return false;

	if(!InitTexture())
		return false;



	return true;
}

bool TextureApp::InitBuffer()
{

	light.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	HeightMapInfo hmInfo;
	HeightMapLoad("heightmap.bmp", hmInfo);		// Load the heightmap and store it into hmInfo

	int cols = hmInfo.terrainWidth;
	int rows = hmInfo.terrainHeight;

	//Create the grid
	NumVertices = rows * cols;
	NumFaces  = (rows-1)*(cols-1)*2;

	std::vector<Vertex> v(NumVertices);

	for(DWORD i = 0; i < rows; ++i)
	{
		for(DWORD j = 0; j < cols; ++j)
		{
			v[i*cols+j].pos = hmInfo.heightMap[i*cols+j];
			v[i*cols+j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	std::vector<DWORD> indices(NumFaces * 3);

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;
	for(DWORD i = 0; i < rows-1; i++)
	{
		for(DWORD j = 0; j < cols-1; j++)
		{
			indices[k]   = i*cols+j;		// Bottom left of quad
			v[i*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			indices[k+1] = (i+1)*cols+j;	// Top left of quad
			v[(i+1)*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k+2] = i*cols+j+1;		// Bottom right of quad
			v[i*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);


			indices[k+3] = (i+1)*cols+j;	// Top left of quad
			v[(i+1)*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k+4] = (i+1)*cols+j+1;	// Top right of quad
			v[(i+1)*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			indices[k+5] = i*cols+j+1;		// Bottom right of quad
			v[i*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			k += 6; // next quad

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex++;
	}

	/************************************New Stuff****************************************************/
	// Since our terrain will not be transformed throughout our scene, we will set the our groundWorlds
	// world matrix here so that when we put the terrains positions in the "polygon soup", they will
	// already be transformed to world space
	groundWorld = XMMatrixIdentity();
	Scale = XMMatrixScaling( 10.0f, 10.0f, 10.0f );
	Translation = XMMatrixTranslation( -520.0f, -10.0f, -1020.0f );
	groundWorld = Scale * Translation;

	// Store the terrains vertex positions and indices in the
	// polygon soup that we will check for collisions with
	// We can store ALL static (non-changing) geometry in here that we want to check for collisions with
	int vertexOffset = collidableGeometryPositions.size();	// Vertex offset (each "mesh" will be added to the end of the positions array)

	// Temp arrays because we need to store the geometry in world space
	XMVECTOR tempVertexPosVec;
	XMFLOAT3 tempVertF3;

	// Push back vertex positions to the polygon soup
	for(int i = 0; i < v.size(); i++)
	{
		tempVertexPosVec = XMLoadFloat3(&v[i].pos);
		tempVertexPosVec = XMVector3TransformCoord(tempVertexPosVec, groundWorld);
		XMStoreFloat3(&tempVertF3, tempVertexPosVec);
		collidableGeometryPositions.push_back(tempVertF3);
	}

	// Push back indices for polygon soup. We need to make sure we are
	// pushing back the indices "on top" of the previous pushed back
	// objects vertex positions, hence "+ vertexOffset" (This is the
	// first object we are putting in here, so it really doesn't
	// matter right now, but I just wanted to show you how to do it
	for(int i = 0; i < indices.size(); i++)
	{
		collidableGeometryIndices.push_back(indices[i] + vertexOffset);
	}
	/*************************************************************************************************/

	//////////////////////Compute Normals///////////////////////////
	//Now we will compute the normals for each vertex using normal averaging
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Used to get vectors (sides) from the position of the verts
	float vecX, vecY, vecZ;

	//Two edges of our triangle
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute face normals
	for(int i = 0; i < NumFaces; ++i)
	{
		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = v[indices[(i*3)]].pos.x - v[indices[(i*3)+2]].pos.x;
		vecY = v[indices[(i*3)]].pos.y - v[indices[(i*3)+2]].pos.y;
		vecZ = v[indices[(i*3)]].pos.z - v[indices[(i*3)+2]].pos.z;		
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

		//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = v[indices[(i*3)+2]].pos.x - v[indices[(i*3)+1]].pos.x;
		vecY = v[indices[(i*3)+2]].pos.y - v[indices[(i*3)+1]].pos.y;
		vecZ = v[indices[(i*3)+2]].pos.z - v[indices[(i*3)+1]].pos.z;		
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

		//Cross multiply the two edge vectors to get the un-normalized face normal
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
		tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
	}

	//Compute vertex normals (normal Averaging)
	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX;
	float tY;
	float tZ;

	//Go through each vertex
	for(int i = 0; i < NumVertices; ++i)
	{
		//Check which triangles use this vertex
		for(int j = 0; j < NumFaces; ++j)
		{
			if(indices[j*3] == i ||
				indices[(j*3)+1] == i ||
				indices[(j*3)+2] == i)
			{
				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum
				facesUsing++;
			}
		}

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = normalSum / facesUsing;

		//Normalize the normalSum vector
		normalSum = XMVector3Normalize(normalSum);

		//Store the normal in our current vertex
		v[i].normal.x = XMVectorGetX(normalSum);
		v[i].normal.y = XMVectorGetY(normalSum);
		v[i].normal.z = XMVectorGetZ(normalSum);

		//Clear normalSum and facesUsing for next vertex
		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		facesUsing = 0;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth      = sizeof(DWORD) * NumFaces * 3;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = &indices[0];
	pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumVertices;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 
	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &v[0];
	hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

	return true;
}

bool TextureApp::InitTexture()
{
	return true;
}

bool TextureApp::InitStatus()
{
	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width    = Width;
	viewport.Height   = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pD3D11DeviceContext->RSSetViewports(1, &viewport);

	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage          = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth      = sizeof(cbPerObject);
	cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags      = 0;

	hr = pD3D11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage          = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth      = sizeof(cbPerFrame);
	cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags      = 0;
	hr                  = pD3D11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

	//Camera information
	camPosition = XMVectorSet( 0.0f, 5.0f, -8.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)Width/Height, 1.0f, 1000.0f);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	HR( D3DX11CreateShaderResourceViewFromFile( pD3D11Device, L"../common/media/texture/grass.jpg",
		NULL, NULL, &CubesTexture, NULL ) );

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD         = 0;
	sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = pD3D11Device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );

	pD3D11Device->CreateBlendState(&blendDesc, &d2dTransparency);

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = pD3D11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	hr = pD3D11Device->CreateRasterizerState(&cmdesc, &CWcullMode);

	return true;
}


///////////////**************new**************////////////////////
void StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart-frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if(tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount)/countsPerSecond;
}


void TextureApp::UpdateScene(double time)
{

}

void TextureApp::RenderText(std::wstring text, int inInt)
{	
	pD3D11DeviceContext->PSSetShader(pD2D_PS, 0, 0);
	pkeyedMutex11->ReleaseSync(0);
	pkeyedMutex10->AcquireSync(0, 5);			

	pD2DRenderTarget->BeginDraw();	
	pD2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Create our string
	std::wostringstream printString; 
	printString << text << inInt;;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	pBrush->SetColor(FontColor);	

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//Draw the Text
	pD2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		pTextFormat,
		layoutRect,
		pBrush
		);

	pD2DRenderTarget->EndDraw();	

	pkeyedMutex10->ReleaseSync(1);
	pkeyedMutex11->AcquireSync(1, 5);

	pD3D11DeviceContext->OMSetBlendState(d2dTransparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	pD3D11DeviceContext->IASetIndexBuffer(pD2DIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &pD2DVertBuffer, &stride, &offset );

	WVP =  XMMatrixIdentity();
	cbPerObj.World = XMMatrixTranspose(WVP);
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &pD2DTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	//pD3D11DeviceContext->RSSetState(CWcullMode);
	//Draw the second cube
	pD3D11DeviceContext->DrawIndexed( 6, 0, 0 );	
}


bool TextureApp::InitShader()
{
	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &pVS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &pPS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "D2D_PS", "ps_4_0", 0, 0, 0, &pD2D_PS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = pD3D11Device->CreateVertexShader(pVS_Buffer->GetBufferPointer(), pVS_Buffer->GetBufferSize(), NULL, &pVS);
	hr = pD3D11Device->CreatePixelShader(pPS_Buffer->GetBufferPointer(), pPS_Buffer->GetBufferSize(), NULL, &pPS);
	hr = pD3D11Device->CreatePixelShader(pD2D_PS_Buffer->GetBufferPointer(), pD2D_PS_Buffer->GetBufferSize(), NULL, &pD2D_PS);

	//Set Vertex and Pixel Shaders
	pD3D11DeviceContext->VSSetShader(pVS, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPS, 0, 0);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);
	hr = pD3D11Device->CreateInputLayout( layout, numElements, pVS_Buffer->GetBufferPointer(), 
		pVS_Buffer->GetBufferSize(), &pInputLayout );

	pD3D11DeviceContext->IASetInputLayout( pInputLayout );
	pD3D11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	return true;
}

void TextureApp::RenderScene()
{

	float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};
	pD3D11DeviceContext->ClearRenderTargetView(pRenderTargetView, bgColor);	
	pD3D11DeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	cbPerLight.light = light;
	pD3D11DeviceContext->UpdateSubresource( cbPerFrameBuffer, 0, NULL, &cbPerLight, 0, 0 );
	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);	
	pD3D11DeviceContext->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );
	pD3D11DeviceContext->OMSetBlendState(0, 0, 0xffffffff);
	pD3D11DeviceContext->VSSetShader(pVS, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPS, 0, 0);

	pD3D11DeviceContext->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the cubes vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );

	WVP = groundWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	cbPerObj.World = XMMatrixTranspose(groundWorld);
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &CubesTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );
	pD3D11DeviceContext->RSSetState(CWcullMode);
	pD3D11DeviceContext->DrawIndexed( NumFaces * 3, 0, 0 );


	RenderText(L"FPS: ", fps);

	//Present the backbuffer to the screen
	pSwapChain->Present(0, 0);
}