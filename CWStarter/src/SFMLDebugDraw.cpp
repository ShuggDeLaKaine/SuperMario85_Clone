#include "SFMLDebugDraw.h"

void SFMLDebugDraw::setWorld(b2World * world) {
	m_pWorld = world;
	m_pWorld->SetDebugDraw(&m_debugDraw);
	m_debugDraw.SetFlags(b2Draw::e_shapeBit);
}
void SFMLDebugDraw::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	m_pWorld->DrawDebugData();
	std::vector<sf::VertexArray> shapes = m_debugDraw.getShapes();
	for (auto shape : shapes)
	{
		target.draw(shape);
	}
};

void SFMLDebugDraw::clear() { m_debugDraw.clearShapes(); }

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
	sf::VertexArray va(sf::Points, 1);
	va[0] = sf::Vertex(sf::Vector2f(p.x, p.y), sf::Color(color.r, color.g, color.b));
	m_shapes.push_back(va);
};

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	sf::VertexArray va(sf::Quads, vertexCount);

	for (int i = 0; i < vertexCount; i++)
	{
		va[i] = sf::Vertex(sf::Vector2f(vertices[i].x, vertices[i].y), sf::Color(color.r, color.g, color.b));
	}
	m_shapes.push_back(va);
};

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	sf::VertexArray va(sf::LineStrip, vertexCount + 1);

	for (int i = 0; i < vertexCount; i++)
	{
		va[i] = sf::Vertex(sf::Vector2f(vertices[i].x, vertices[i].y), sf::Color(color.r, color.g, color.b));
	}
	va[vertexCount] = va[0];
	m_shapes.push_back(va);
};

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	float vertexCount = 30.f;
	sf::VertexArray va(sf::LinesStrip, vertexCount);
	float thetaStep = (2.f * 3.15f) / (vertexCount - 1.f);
	float theta = 0;
	sf::Vector2f point;

	for (int i = 0; i < vertexCount; i++)
	{
		point.x = center.x + cos(theta) * radius;
		point.y = center.y + sin(theta) * radius;
		va[i] = sf::Vertex(point, sf::Color(color.r, color.g, color.b));
		theta += thetaStep;
	}
	m_shapes.push_back(va);
};

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	float vertexCount = 31.f;
	sf::VertexArray va(sf::TriangleFan, vertexCount);
	float thetaStep = (2.f * 3.15f) / (vertexCount - 2.f);
	float theta = 0;
	sf::Vector2f point;

	va[0] = sf::Vertex(sf::Vector2f(center.x, center.y), sf::Color(color.r, color.g, color.b));
	for (int i = 1; i < vertexCount; i++)
	{
		point.x = center.x + cos(theta) * radius;
		point.y = center.y + sin(theta) * radius;
		va[i] = sf::Vertex(point, sf::Color(color.r, color.g, color.b));
		theta += thetaStep;
	}
	m_shapes.push_back(va);

	sf::VertexArray va2(sf::Lines, 2);
	va2[0] = sf::Vertex(sf::Vector2f(center.x, center.y), sf::Color(255, 0, 0));
	point.x = center.x + axis.x * radius;
	point.y = center.y + axis.y * radius;
	va2[1] = sf::Vertex(point, sf::Color(255, 0, 0));
	m_shapes.push_back(va2);
};

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	sf::VertexArray va(sf::Lines, 2);
	va[0] = sf::Vertex(sf::Vector2f(p1.x, p1.y), sf::Color(color.r, color.g, color.b));
	va[1] = sf::Vertex(sf::Vector2f(p2.x, p2.y), sf::Color(color.r, color.g, color.b));
	m_shapes.push_back(va);
};

void DebugDraw::DrawTransform(const b2Transform& xf) {
	// Sorry, not sure how to do this?
};