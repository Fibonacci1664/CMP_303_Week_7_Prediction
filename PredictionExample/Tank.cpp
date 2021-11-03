#include "Tank.h"

Tank::Tank(std::string color) : sf::Sprite()
{
	m_BodyTexture.loadFromFile("Assets/" + color + "Tank.png");
	m_BarrelTexture.loadFromFile("Assets/" + color + "Barrel.png");
	setTexture(m_BodyTexture);

	setOrigin(getTextureRect().width / 2, getTextureRect().height / 2);

	m_GhostSprite.setTexture(m_BodyTexture);
	m_GhostSprite.setColor(sf::Color(255, 255, 255, 128));
	m_GhostSprite.setOrigin(getTextureRect().width / 2, getTextureRect().height / 2);
	setGhostPosition( getPosition() );

	m_BarrelSprite.setTexture(m_BarrelTexture);
	m_BarrelSprite.setOrigin(6, 2);
	m_BarrelSprite.setPosition( getPosition() );
}


Tank::~Tank()
{
}

//Sets the tank's position to the latest network position
void Tank::Update(float dt)
{
	if (m_Messages.size() < 1)
		return;

	TankMessage latestMessage = m_Messages.back();
	setPosition( latestMessage.x, latestMessage.y );
}

void Tank::setPosition( float x, float y )
{
	sf::Sprite::setPosition(x, y);
	m_BarrelSprite.setPosition(getPosition());
}

//Use this to set the prediction position
void Tank::setGhostPosition( sf::Vector2f pos )
{
	m_GhostSprite.setPosition( pos );
}

//Draw the tank / or the ghost / or both
const void Tank::Render(sf::RenderWindow * window)
{
	if(m_RenderMode > 0)
		window->draw(m_GhostSprite);

	if (m_RenderMode != 1)
	{
		window->draw(*this);
		window->draw(m_BarrelSprite);
	}
}

//Add a message to the tank's network message queue
void Tank::AddMessage(const TankMessage & msg)
{
	m_Messages.push_back(msg);
}

//This method calculates and stores the position, but also returns it immediately for use in the main loop
//This is my where prediction would be... IF I HAD ANY
sf::Vector2f Tank::RunPrediction(float gameTime)
{
	float predictedX = -1.0f;
	float predictedY = -1.0f;

	const int msize = m_Messages.size();

	if( msize < 3 )
	{
		return sf::Vector2f( predictedX, predictedX );
	}

	const TankMessage& msg0 = m_Messages[msize - 1];		// Latest msg
	const TankMessage& msg1 = m_Messages[msize - 2];
	const TankMessage& msg2 = m_Messages[msize - 3];		// Oldest msg
	
	// FIXME: Implement prediction here!
	// You have:
	// - the history of position messages received, in "m_Messages"
	//   (msg0 is the most recent, msg1 the 2nd most recent, msg2 the 3rd most recent)
	// - the current time, in "gameTime"
	//
	// You need to update:
	// - the predicted position at the current time, in "predictedX" and "predictedY"

	// NO MODEL TASK
	/*predictedX = msg0.x;
	predictedY = msg0.y;*/

	// LINEAR MODEL TASK (VEL ONLY) using s = vt
	/*sf::Vector2f prevPrevPos = sf::Vector2f(msg1.x, msg1.y);
	sf::Vector2f prevPos = sf::Vector2f(msg0.x, msg0.y);
	sf::Vector2f distanceVec = prevPos - prevPrevPos;
	float dist = std::hypot(distanceVec.x, distanceVec.y);
	float deltaTime = msg0.time - msg1.time;
	float speed = dist / deltaTime;
	float displacement = speed * (gameTime - msg0.time);
	sf::Vector2f nextPos = sf::Vector2f(msg0.x + displacement, msg0.y + displacement);
	predictedX = nextPos.x;
	predictedY = nextPos.y;*/

	// QUADRACTIC MODEL TASK (USING ACC AND VEL) using s = ut + 0.5 * at^2
	sf::Vector2f prevPrevPrevPos = sf::Vector2f(msg2.x, msg2.y);
	sf::Vector2f prevPrevPos = sf::Vector2f(msg1.x, msg1.y);
	sf::Vector2f prevPos = sf::Vector2f(msg0.x, msg0.y);
	sf::Vector2f prevDistanceVec = prevPos - prevPrevPos;
	sf::Vector2f prevPrevDistanceVec = prevPrevPos - prevPrevPrevPos;
	float prevDist = std::hypot(prevDistanceVec.x, prevDistanceVec.y);
	float prevPrevDist = std::hypot(prevPrevDistanceVec.x, prevPrevDistanceVec.y);
	float prevDeltaTime = msg0.time - msg1.time;
	float prevPrevDeltaTime = msg1.time - msg2.time;
	float prevSpeed = prevDist / prevDeltaTime;
	float prevPrevSpeed = prevPrevDist / prevPrevDeltaTime;
	float deltaSpeed = prevSpeed - prevPrevSpeed;
	float deltaTime = prevDeltaTime - prevPrevDeltaTime;
	float accel = deltaSpeed / deltaTime;
	float displacement = prevSpeed * (gameTime - msg0.time) + 0.5f * accel * pow((gameTime - msg0.time), 2);
	sf::Vector2f nextPos = sf::Vector2f(msg0.x + displacement, msg0.y + displacement);
	predictedX = nextPos.x;
	predictedY = nextPos.y;



	return sf::Vector2f( predictedX, predictedY );
}

void Tank::Reset()
{
	m_Messages.clear();
}
