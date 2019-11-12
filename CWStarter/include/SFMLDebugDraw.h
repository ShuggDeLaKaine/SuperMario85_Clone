#pragma once

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

class DebugDraw : public b2Draw { // b2Draw has all the virtual functions that we need to override here
private:
	std::vector<sf::VertexArray> m_shapes; //!< SFML shapes (made from vertexarrays) to be drawn to the screen
public:
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color); //!< Draw a point (dot)
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color); //!< Draw a filled polygon
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color); //!< Draw the outline of a polygon
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color); //!< Draw a circle outline
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color); //!< Draw a filled circle with axis
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color); //!< Draw line segment
	void DrawTransform(const b2Transform& xf); //!< Drawa transform - not implemented

	std::vector<sf::VertexArray> getShapes() const { return m_shapes; }; //!< Return the shapes to be drawn
	void clearShapes() {m_shapes.clear();}; //!< Clear shape buffer
};

class SFMLDebugDraw : public sf::Drawable 
{
private:
	b2World * m_pWorld = nullptr; //!< Pointer to the Box2D world.  Using pointers as BOX2D has it's own memory management
	DebugDraw m_debugDraw; //!< Debug draw class
public:
	void setWorld(b2World * world); //!< Set the world pointer
	void draw(sf::RenderTarget &target, sf::RenderStates states) const; //!< Draw debug shapes - overidden from drawable
	void clear(); //!< Clear shape buffer
};

