using namespace std;
#pragma comment(lib, "glfw3.lib")
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <memory>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.hpp"
#include "stb_image.h"

const GLuint WIDTH = 1280, HEIGHT = 720;

struct Vertex
{
	float x, y, z;
	float r, g, b;
	float s, t;
	Vertex(float x, float y, float z, float r, float g, float b, float s, float t)
		:x(x), y(y), z(z), r(r), g(g), b(b), s(s), t(t) {}
};

glm::vec3 KeyboardMoveController(GLFWwindow* Window, glm::vec3 pos)
{
	glm::vec3 result;
	
	    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS && pos.x >= -40)
	    {
			result.x += -1;
			//std::cout << "x-";
	    }
		
		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS && pos.x <= 40)
		{
			result.x += 1;
			//std::cout << "x+";
	    }
		
		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS && pos.y >= -40 )
		{
			result.y += -1;
			//std::cout << "y-";
	    }
		
		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS && pos.y <= 40)
		{
			result.y += 1;
			//std::cout << "y+";
	    }
		
	return result;
};
int KeyboardADController(GLFWwindow* Window)
{
	int result=0;

	if (glfwGetKey(Window, GLFW_KEY_J) == GLFW_PRESS)
	{
		result = 1;
	}

	if (glfwGetKey(Window, GLFW_KEY_K) == GLFW_PRESS)
	{
		result = 2;
	}
	return result;
};
bool KeyboardRestarController(GLFWwindow* Window)
{
	bool result = false;

	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
	{
		result = true;
	}
	return result;
};

class Drawable
{
protected:

	unsigned m_vao;
	
public:

	Drawable()
	{
		m_vao = 0;
	}
	~Drawable()
	{
		glDeleteVertexArrays(1, &m_vao);
	}
	virtual void CreateVAO() = 0;
	virtual void Draw(ShaderProgram& shader) = 0;
};
class Shield : public Drawable
{
private:
	glm::vec3 p;
	std::vector<Vertex> vertices;
	float r;
	float g;
	float b;
	int HP = 5;
	bool live;
public:
	Shield(const glm::vec3& pos) :
		Drawable(),
		live(true),
		p(pos)
	{
		float w = 3.5f;
		float h = 1.f;
		r = 0.55f;
		g = 0.67f;
		b = 0.65f;

		while (h >= 0)
		{
			vertices.push_back(Vertex(-w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(-w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(-w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(-w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			h -= 0.2;
		}
		h = 1.f;
		while (w >= 0)
		{
			vertices.push_back(Vertex(-w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(-w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(-w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			vertices.push_back(Vertex(-w / 2, -h / 2, 0, r, g, b, 0.0f, 0.0f));
			w -= 0.2;
		}
	}

	glm::vec3& Position() { return p; }
	void SetPosition(glm::vec3& ps) { p = ps; };
	void Hit() 
	{ 
		HP--; 
		if (HP<=0)
		{
			Dead();
		}
	}
	bool ALive() const { return live; }
	void Dead() { live = false; }
	void Revive() 
	{ 
		HP = 5;
		live = true; 
	}

	float XMax() const { return p.x + 5.25f; }
	float XMin() const { return p.x - 5.25f; }
	float YMax() const { return p.y + 1.5f; }
	float YMin() const { return p.y - 1.5f; }
	float ZMax() const { return p.z; }
	float ZMin() const { return p.z; }

	void Animate(float m_speedCoef, glm::vec3 d, float timeDelta)
	{
		p += timeDelta*m_speedCoef * d;
	}
	virtual void CreateVAO()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
	virtual void Draw(ShaderProgram& shader)
	{
		glm::vec3 otherColor(0.55f, 0.67f, 0.65f);
		if (HP == 3)
		{
			otherColor = glm::vec3(1.f, 0.f, 0.76f);
		}
		if (HP == 1)
		{
			otherColor = glm::vec3(0.23f, 0.36f, 0.74f);
		}
		if (ALive() == true)
		{
			glBindVertexArray(m_vao);
			glm::mat4 model = glm::mat4(2.0f);
			model = glm::translate(model, p);
			model = glm::scale(model, glm::vec3(1.5, 1.5, 0));
			int colorSwitch = 3;
			glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);

			glUniform3fv(glGetUniformLocation(shader.ID, "otherColor"), 1, glm::value_ptr(otherColor));
			float proporcia = 0.f;
			glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}
	}
};
class Laser : public Drawable
{
private:
	glm::vec3 p;
	std::vector<Vertex> vertices;
	float my_speedCoef;
	float r;
	float g;
	float b;
	bool ready;
public:	
	Laser(const glm::vec3& pos) :
		Drawable(),
		p(pos),
		my_speedCoef(200),
		ready(true)
	{
		r = 0.8f;
		g = 0.f;
		b = 0.65f;
		//Korpus
		//1 ABC BDC
		//A
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		//B												   
		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, r, g, b, 1.f, 0.5f));
		//C												   
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, r, g, b, 0.5f, 1.f));

		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, r, g, b, 1.0f, 0.5f));
		//D												   
		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, r, g, b, 1.f, 1.f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, r, g, b, 0.5f, 1.f));

		//2 FKE KGE										   
		//F												   
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, r, g, b, 0.0f, 0.5f));
		//K											 	   
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, r, g, b, 0.5f, 0.5f));
		//E											 	   
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, r, g, b, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, r, g, b, 0.5f, 0.5f));
		//G											 	   
		vertices.push_back(Vertex(0.5f, 0.5f, 4.f, r, g, b, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, r, g, b, .0f, 1.0f));

		//3 FAE ACE									 	   
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, r, g, b, 0.0f, 0.5f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, r, g, b, .0f, 1.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, r, g, b, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, r, g, b, .0f, 1.0f));

		//4 CDE DGE
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, r, g, b, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, r, g, b, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 4.f, r, g, b, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, r, g, b, .0f, 1.0f));

		//5 ABF BKF									   	  
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, r, g, b, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, r, g, b, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, r, g, b, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, r, g, b, .0f, 1.0f));

		//6 BDK DGK									   	  
		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, r, g, b, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, r, g, b, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, r, g, b, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 4.f, r, g, b, 0.5f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, r, g, b, .0f, 1.0f));
	}
	Laser() {}
	glm::vec3& Position() { return p; }
	void SetPosition(glm::vec3& ps) { p = ps; };
	bool Ready() { return ready; }
	void setReady(bool s) {ready = s; }

	float XMax() const { return p.x + 0.2f; }
	float XMin() const { return p.x - 0.2f; }
	float YMax() const { return p.y + 0.2f; }
	float YMin() const { return p.y - 0.2f; }
	float ZMax() const { return p.z + 2.4f; }
	float ZMin() const { return p.z; }

	void ReCharge(glm::vec3 shipP)
	{
		p = shipP;
		setReady(true);
	}
	void Animate(float m_speedCoef, glm::vec3 d, int a, float timeDelta)
	{
		if (ready==true)
		{
			p += timeDelta*m_speedCoef * d;
		}
		else
		{
			if (a == 1)
			{
				p += timeDelta*my_speedCoef * glm::vec3(0, 0, 1);
			}
			else if(a == 2)
			{
				p += timeDelta*my_speedCoef * glm::vec3(0, 0,-1);
			}		
		}		
	}
	virtual void CreateVAO()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
	virtual void Draw(ShaderProgram& shader)
	{
		if (Ready() == false)
		{
			glBindVertexArray(m_vao);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, p);
			model = glm::scale(model, glm::vec3(0.4, 0.4, 0.6));
			model = glm::rotate(model, 50 * (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
			int colorSwitch = 3;
			glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
			float proporcia = 1.f;
			glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			glBindVertexArray(0);
		}
	}
};
class SS1 : public Drawable
{
private:
	Shield myShield;
	Laser myLaser;
	glm::vec3 p;
	glm::vec3 oldp;
	std::vector<Vertex> vertices;
	glm::vec3 d;
	float m_speedCoef;
	bool live;
public:
	
	SS1(const glm::vec3& pos) :
		Drawable(),
		p(pos),
		d(0.0f, 0.0f, 0.0f),
		m_speedCoef(20),
		live(true),
		myShield(Shield(glm::vec3(pos.x, pos.y, pos.z + 4))),
		myLaser(Laser(glm::vec3(pos.x, pos.y, pos.z + 4)))
	{
		//Korpus
		//1 ABC BDC
		//A
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//B
		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 1.f, 0.5f));
		//C
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 1.f));

		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, 0.5f));
		//D
		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 1.f, 1.f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 1.f));

		//2 FKE KGE
		//F
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		//K
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//E
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//G
		vertices.push_back(Vertex(0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//3 FAE ACE
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//4 CDE DGE
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//5 ABF BKF
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//6 BDK DGK
		vertices.push_back(Vertex(0.5f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.5f, 4.f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 4.f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//lqv trystar dyrjach
		//1 ABC BDC
		//A
		vertices.push_back(Vertex(-0.75f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		//B
		vertices.push_back(Vertex(-0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//C
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.125f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(-0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//D
		vertices.push_back(Vertex(-0.5f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.125f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//2 FKE KGE
		//F
		vertices.push_back(Vertex(-0.75f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		//K
		vertices.push_back(Vertex(-0.5f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//E
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(-0.5f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//G
		vertices.push_back(Vertex(-0.5f, 0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//4 CDE DGE
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(-0.5f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, 0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//5 ABF BKF
		vertices.push_back(Vertex(-0.75f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(-0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.75f, -0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(-0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(-0.5f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(-0.75f, -0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//desen trystar dyrjach
		//1 ABC BDC
		//A
		vertices.push_back(Vertex(0.75f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		//B
		vertices.push_back(Vertex(0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//C
		vertices.push_back(Vertex(0.75f, 0.25f, 0.125f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//D
		vertices.push_back(Vertex(0.5f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(0.75f, 0.25f, 0.125f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//2 FKE KGE
		//F
		vertices.push_back(Vertex(0.75f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		//K
		vertices.push_back(Vertex(0.5f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//E
		vertices.push_back(Vertex(0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		//G
		vertices.push_back(Vertex(0.5f, 0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//4 CDE DGE
		vertices.push_back(Vertex(0.75f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, 0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, 0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(0.75f, 0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//5 ABF BKF
		vertices.push_back(Vertex(0.75f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.0f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.75f, -0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		vertices.push_back(Vertex(0.5f, -0.25f, 0.125f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
		vertices.push_back(Vertex(0.5f, -0.25f, 0.375f, .20f, 0.0f, 0.f, 0.5f, 1.0f));
		vertices.push_back(Vertex(0.75f, -0.25f, 0.375f, .20f, 0.0f, 0.f, .0f, 1.0f));

		//lqv trystar 
		//1 ABC BDC
		//A
		vertices.push_back(Vertex(-0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		//B
		vertices.push_back(Vertex(-1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		//C
		vertices.push_back(Vertex(-0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, .5f, .5f));

		vertices.push_back(Vertex(-1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		//D
		vertices.push_back(Vertex(-1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, .5f));
		vertices.push_back(Vertex(-0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, .5f, .5f));

		//2 FKE KGE
		////F
		//vertices.push_back(Vertex(-0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		////K
		//vertices.push_back(Vertex(-1.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		////E
		//vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//vertices.push_back(Vertex(-1.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		////G
		//vertices.push_back(Vertex(-1.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		//vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//3 FAE ACE
		vertices.push_back(Vertex(-0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		vertices.push_back(Vertex(-0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(-0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//4 CDE DGE
		vertices.push_back(Vertex(-0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//vertices.push_back(Vertex(-1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		//vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		//vertices.push_back(Vertex(-1.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//5 ABF BKF
		vertices.push_back(Vertex(-0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(-0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//vertices.push_back(Vertex(-1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		//vertices.push_back(Vertex(-1.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		//vertices.push_back(Vertex(-0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//6 BDK DGK
		vertices.push_back(Vertex(-1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(-0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		vertices.push_back(Vertex(-1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(-0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		vertices.push_back(Vertex(-0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//desen trystar 
		//1 ABC BDC
		//A
		vertices.push_back(Vertex(0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		//B
		vertices.push_back(Vertex(1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		//C
		vertices.push_back(Vertex(0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, .5f, .5f));

		vertices.push_back(Vertex(1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		//D
		vertices.push_back(Vertex(1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 1.0f, .5f));
		vertices.push_back(Vertex(0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, .5f, .5f));

		//2 FKE KGE
		////F
		//vertices.push_back(Vertex(0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		////K
		//vertices.push_back(Vertex(1.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		////E
		//vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//vertices.push_back(Vertex(1.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		////G
		//vertices.push_back(Vertex(1.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		//vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//3 FAE ACE
		vertices.push_back(Vertex(0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		vertices.push_back(Vertex(0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//4 CDE DGE
		vertices.push_back(Vertex(0.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		/*vertices.push_back(Vertex(1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		vertices.push_back(Vertex(1.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));*/

		//5 ABF BKF
		vertices.push_back(Vertex(0.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//vertices.push_back(Vertex(1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		//vertices.push_back(Vertex(1.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		//vertices.push_back(Vertex(0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		//6 BDK DGK
		vertices.push_back(Vertex(1.75f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));

		vertices.push_back(Vertex(1.75f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.0f));
		vertices.push_back(Vertex(0.75f, 0.5f, 2.f, .20f, 0.0f, 0.f, 0.5f, .5f));
		vertices.push_back(Vertex(0.75f, -0.5f, 2.f, .20f, 0.0f, 0.f, .0f, .5f));
	}

	glm::vec3& Direction() { return d; }
	void SetDirection(glm::vec3& dir) { d=dir; }
	glm::vec3& Position() { return p; }
	void SetPosition(glm::vec3& ps) { p = ps; };
	glm::vec3& oldPosition() { return oldp; }
	float Speed()  { return m_speedCoef; }
	Shield& getShild() { return myShield; }
	Laser& getLaser() { return myLaser; }
	bool ALive() const { return live; }
	void Dead() { live = false; }
	void Revive() { live = true; }

	float XMax() const { return p.x + 1.75f; }
	float XMin() const { return p.x - 1.75f; }
	float YMax() const { return p.y + 0.5f; }
	float YMin() const { return p.y - 0.5f; }
	float ZMax() const { return p.z + 4 ; }
	float ZMin() const { return p.z; }

	void Move(GLFWwindow* Window)
	{		
		d = KeyboardMoveController(Window, p);		
	}
	void Animate(float timeDelta)
	{		
		myShield.Animate(m_speedCoef,d, timeDelta);
		myLaser.Animate(m_speedCoef,d,1, timeDelta);
		p += timeDelta*m_speedCoef * d;
	}
	virtual void CreateVAO()
	{
		myShield.CreateVAO();
		myLaser.CreateVAO();
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);	
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pData = std::shared_ptr<unsigned char>(stbi_load("Resources/SpaceShip1.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			if (!pData)
				throw std::exception("Failed to load texture");

			unsigned texture;
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData.get());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	virtual void Draw(ShaderProgram& shader)
	{
		
		glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 0);
		glBindVertexArray(m_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, p);
		//model = glm::rotate(model,  (float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
		if (oldp.x<p.x && p.x >= -40 && p.x <= 40)
		{
			model = glm::rotate(model, glm::radians(5.0f), glm::vec3(0.f, 1.f, 0.f));
		}
		else if (oldp.x > p.x && p.x >= -40 && p.x <= 40)
		{
			model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.f, 1.f, 0.f));
		}
		if (oldp.y < p.y && p.y >= -40 && p.y <= 40)
		{
			model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(1.f, 0.f, 0.f));
		}
		else if (oldp.y > p.y && p.y >= -40 && p.y <= 40)
		{
			model = glm::rotate(model, glm::radians(5.0f), glm::vec3(1.f, 0.f, 0.f));
		}
		float proporcia = 0.f;
		int colorSwitch = 1;
		glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
		glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
		oldp = p;
	}
};
class SS2 : public Drawable
{
private:
	Laser myLaser;
	glm::vec3 p;
	std::vector<Vertex> vertices;
	float m_speedCoef;
	bool live;
public:
	SS2(const glm::vec3& pos) :
		Drawable(),
		p(pos),
		m_speedCoef(10),
		live(true),
		myLaser(Laser(glm::vec3(pos.x, pos.y, pos.z - 11)))
	{
		//Korpus
		//BCF CDF
		//B
		vertices.push_back(Vertex(1.f, .0f, 0.f, .20f, 0.0f, 0.f, .5f, .5f));
		//C
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.f, .5f));
		//F
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.f));

		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.f, .5f));
		//D
		vertices.push_back(Vertex(-1.f, -0.f, 0.f, .20f, 0.0f, 0.f, 0.f, .0f));
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.f));

		//BAC
		vertices.push_back(Vertex(1.f, .0f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.f));
		//A
		vertices.push_back(Vertex(0.f, .0f, 3.f, .20f, 0.0f, 0.f, 1.f, 0.f));
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, .5f));

		//CAD
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.f));
		vertices.push_back(Vertex(0.f, .0f, 3.f, .20f, 0.0f, 0.f, 1.f, 0.f));
		vertices.push_back(Vertex(-1.f, -0.f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));

		//DAF
		vertices.push_back(Vertex(-1.f, -0.f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.f));
		vertices.push_back(Vertex(0.f, .0f, 3.f, .20f, 0.0f, 0.f, 1.f, 0.f));
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));

		//FAB
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.f));
		vertices.push_back(Vertex(0.f, .0f, 3.f, .20f, 0.0f, 0.f, 1.f, 0.f));
		vertices.push_back(Vertex(1.f, .0f, 0.f, .20f, 0.0f, 0.f, 0.5f, 0.5f));
	}

	Laser& getLaser() { return myLaser; }
	glm::vec3& Position() { return p; }
	void SetPosition(glm::vec3& ps) { p = ps; };
	bool ALive() const { return live; }
	void Dead() { live = false; }
	void Revive() { live = true; }

	float XMax() const { return p.x + 3; }
	float XMin() const { return p.x - 3; }
	float YMax() const { return p.y + 1.5f; }
	float YMin() const { return p.y - 1.5f; }
	float ZMax() const { return p.z; }
	float ZMin() const { return p.z - 9.0f; }

	void Animate(glm::vec3 d, float timeDelta)
	{		
		myLaser.Animate(m_speedCoef, d, 2, timeDelta);
		p += timeDelta*m_speedCoef * d;
	}
	virtual void CreateVAO()
	{
		myLaser.CreateVAO();
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);	
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pData = std::shared_ptr<unsigned char>(stbi_load("Resources/SpaceShip2.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			if (!pData)
				throw std::exception("Failed to load texture");

			unsigned texture;
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData.get());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	virtual void Draw(ShaderProgram& shader)
	{
		glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 1);
		glBindVertexArray(m_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, p);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, glm::vec3(3, 3, 3));
		int colorSwitch = 1;
		glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
		float proporcia = 0.f;
		glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
};
class SS3 : public Drawable
{
private:
	glm::vec3 p;
	std::vector<Vertex> vertices;
	glm::vec3 d;
	float m_speedCoef;
	bool live;
public:
	SS3(const glm::vec3& pos) :
		Drawable(),
		p(pos),
		d(0.0f, 0.0f, 0.0f),
		m_speedCoef(20),
		live(true)
	{
		//GORNA CHAS
        //ACD
        //A
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		//C
		vertices.push_back(Vertex(0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//D
		vertices.push_back(Vertex(-0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//ADT
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//T
		vertices.push_back(Vertex(-1.f, 0.f, .5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//ATM
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.f, 0.f, .5f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//M
		vertices.push_back(Vertex(-1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//AMN
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//N
		vertices.push_back(Vertex(-.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//ANF
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//F
		vertices.push_back(Vertex(.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//AFP
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//P
		vertices.push_back(Vertex(1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//APQ
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 1.f, 0.0f));
		//Q
		vertices.push_back(Vertex(1.f, 0.f, .5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//APC
		vertices.push_back(Vertex(0.f, 0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.f, 0.f, .5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//DOLNA CHAS
		//BCD
		//B
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//ADT
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-1.f, 0.f, .5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//ATM
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.f, 0.f, .5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//AMN
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//ANF
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//AFP
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(.5f, 0.f, -1.f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//APQ
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.f, 0.f, -.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(1.f, 0.f, .5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));

		//APC
		vertices.push_back(Vertex(0.f, -0.5f, 0.f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(1.f, 0.f, .5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.f, 1.f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
	}

    glm::vec3& Position() { return p; }
	void SetPosition(glm::vec3& ps) { p = ps; };
	bool ALive() const { return live; }
	void Dead() { live = false; }
	void Revive() { live = true; }

	float XMax() const { return p.x + 3; }
	float XMin() const { return p.x - 3; }
	float YMax() const { return p.y + 1.5f; }
	float YMin() const { return p.y - 1.5f; }
	float ZMax() const { return p.z + 3; }
	float ZMin() const { return p.z - 3; }

	void Animate(glm::vec3 dd, float timeDelta)
	{
		d = dd;
		p += timeDelta*m_speedCoef * d;
	}
	virtual void CreateVAO()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pData = std::shared_ptr<unsigned char>(stbi_load("Resources/SpaceShip3.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			if (!pData)
				throw std::exception("Failed to load texture");

			unsigned texture;
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData.get());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	virtual void Draw(ShaderProgram& shader)
	{

		glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 2);
		glBindVertexArray(m_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, p);

		if (d == glm::vec3(1.f, 0.f, 0.f))
		{
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
		}
		else if (d == glm::vec3(-1.f, 0.f, 0.f))
		{
			model = glm::rotate(model, -(float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
		}	
		model = glm::scale(model, glm::vec3(3, 3, 3));
		int colorSwitch = 1;
		glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
		float proporcia = 0.f;
		glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
};
class SS4 : public Drawable
{
private:
	glm::vec3 p;
	std::vector<Vertex> vertices;
	glm::vec3 d;
	float m_speedCoef;
	bool live;
public:
	SS4(const glm::vec3& pos) :
		Drawable(),
		p(pos),
		d(0.0f, 0.0f, 0.0f),
		m_speedCoef(20),
		live(true)
	{
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
	}

	glm::vec3& Position() { return p; }
	void SetPosition(glm::vec3& ps) { p = ps; };
	bool ALive() const { return live; }
	void Dead() { live = false; }
	void Revive() { live = true; }

	float XMax() const { return p.x + 1.5f; }
	float XMin() const { return p.x - 1.5f; }
	float YMax() const { return p.y + 1.5f; }
	float YMin() const { return p.y - 1.5f; }
	float ZMax() const { return p.z + 1.5f; }
	float ZMin() const { return p.z - 1.5f; }

	void Animate(glm::vec3 dd, float timeDelta)
	{
		d = dd;
		p += timeDelta * m_speedCoef * d;
	}
	virtual void CreateVAO()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pData = std::shared_ptr<unsigned char>(stbi_load("Resources/fire1.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			if (!pData)
				throw std::exception("Failed to load texture");

			unsigned texture;
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData.get());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	virtual void Draw(ShaderProgram& shader)
	{

		glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 3);
		glBindVertexArray(m_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, p);

		if (d == glm::vec3(1.f, 0.f, 0.f))
		{
			model = glm::rotate(model, 100*(float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
		}
		else if (d == glm::vec3(-1.f, 0.f, 0.f))
		{
			model = glm::rotate(model, 100*(float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
		}
		model = glm::scale(model, glm::vec3(3, 3, 3));
		int colorSwitch = 1;
		glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
		float proporcia = 0.f;
		glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
	
};
class Map : public Drawable
{
private:
	glm::vec3 p;
	std::vector<Vertex> vertices;
public:
	Map(const glm::vec3& pos) :
		Drawable(),
		p(pos)
	{
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
	}

	float XMax() const { return p.x + 50.f; }
	float XMin() const { return p.x - 50.f; }
	float YMax() const { return p.y + 50.f; }
	float YMin() const { return p.y - 50.f; }
	float ZMax() const { return p.z + 50.f; }
	float ZMin() const { return p.z - 50.f; }

	virtual void CreateVAO()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pData = std::shared_ptr<unsigned char>(stbi_load("Resources/space1.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			if (!pData)
				throw std::exception("Failed to load texture");

			unsigned texture;
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData.get());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	virtual void Draw(ShaderProgram& shader)
	{
		glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 4);
		glBindVertexArray(m_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, p);
		model = glm::scale(model, glm::vec3(100, 100, 100));
		float proporcia = 0.f;
		glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
		int colorSwitch = 1;
		glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
};
class Explosion : public Drawable
{
private:
	glm::vec3 p;
	std::vector<Vertex> vertices;
public:
	Explosion(const glm::vec3& pos) :
		Drawable(),
		p(pos)
	{
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, -0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));

		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 1.0f, 0.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 1.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, 0.5f, .20f, 0.0f, 0.f, 0.0f, 1.0f));
		vertices.push_back(Vertex(-0.5f, 0.5f, -0.5f, .20f, 0.0f, 0.f, 0.0f, 0.0f));
	}

		void SetPosition(glm::vec3& ps) { p = ps; };
	virtual void CreateVAO()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, s));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		{
			int width, height, nrChannels;
			std::shared_ptr<unsigned char> pData = std::shared_ptr<unsigned char>(stbi_load("Resources/Explosion.jpg", &width, &height, &nrChannels, 0), stbi_image_free);
			if (!pData)
				throw std::exception("Failed to load texture");

			unsigned texture;
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData.get());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	virtual void Draw(ShaderProgram& shader)
	{
		glUniform1i(glGetUniformLocation(shader.ID, "ourTexture"), 5);
		glBindVertexArray(m_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, p);
		model = glm::scale(model, glm::vec3(4, 4, 4));
		float proporcia = 0.f;
		glUniform1f(glGetUniformLocation(shader.ID, "p"), proporcia);
		int colorSwitch = 1;
		glUniform1i(glGetUniformLocation(shader.ID, "colorSwitch"), colorSwitch);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
};
class Game
{
public:
	std::unique_ptr<ShaderProgram> shader;
	vector<unique_ptr<SS2>> enemies1;
	vector<unique_ptr<SS3>> enemies2;
	vector<unique_ptr<SS4>> enemies3;
	vector<bool> moveX1;
	vector< glm::vec3> enemyPos;
	vector< glm::vec3> enemyPos2;
	glm::mat4 view;
	glm::mat4 projection;
	bool moveX2 = false;
	bool moveX3 = false;
	bool gameStatusDef = false;
	int ADInt;	
	int score = 0;

	SS1 myShip= SS1(glm::vec3(0.f, 0.f, -40.f));

	Map map = Map(glm::vec3(0.f, 0.f, 0.f));

	Explosion exp = Explosion(glm::vec3(0.f, 0.f, 0.f));

	void Restart() 
	{
		for (int i = 0; i < enemyPos.size(); i++)
		{
			if (i <= 7)
			{
				enemies1.at(i).get()->SetPosition(enemyPos.at(i));
				enemies1.at(i).get()->Revive();
			}
			else if (i <= 32)
			{
				enemies2.at(i - 8).get()->SetPosition(enemyPos.at(i));
				enemies2.at(i - 8).get()->Revive();
			}
			else {
				enemies3.at(i - 8).get()->SetPosition(enemyPos2.at(i));
				enemies3.at(i - 8).get()->Revive();
			}
		}
		for (int i = 0; i < enemyPos2.size(); i++)
		{
			enemies3.at(i).get()->SetPosition(enemyPos2.at(i));
			enemies3.at(i).get()->Revive();
		}
		myShip.SetPosition(glm::vec3(0.f, 0.f, -40.f));
		myShip.Revive();
		myShip.getShild().SetPosition(myShip.Position() + glm::vec3(0.f, 0.f, 4.f));
		myShip.getShild().Revive();
		myShip.getLaser().ReCharge(myShip.Position());

		view = glm::translate(view, myShip.oldPosition()- glm::vec3(0.f, 0.f, myShip.oldPosition().z));
		SetView(view);
		score = 0;
		gameStatusDef = false;
	}
	void LoadEnemies1()
	{		
		float a = 20;
		float b = 30;
		enemies1.push_back(make_unique<SS2>(glm::vec3(a - 5, b, 40.f)));
		enemies1.push_back(make_unique<SS2>(glm::vec3(-a + 5, b, 40.f)));	
		enemies1.push_back(make_unique<SS2>(glm::vec3(a - 5, b - 20, 40.f)));
		enemies1.push_back(make_unique<SS2>(glm::vec3(-a + 5, b - 20, 40.f)));
		enemies1.push_back(make_unique<SS2>(glm::vec3(a - 5, -b + 20, 40.f)));
		enemies1.push_back(make_unique<SS2>(glm::vec3(-a + 5, -b + 20, 40.f)));
		enemies1.push_back(make_unique<SS2>(glm::vec3(a - 5, -b, 40.f)));
		enemies1.push_back(make_unique<SS2>(glm::vec3(-a + 5, -b, 40.f)));
		moveX1.push_back(false);
		moveX1.push_back(false);
		moveX1.push_back(false);
		moveX1.push_back(false);

	}
	void LoadEnemies2()
	{
		float a = 36;
		float b = 40;
		while (a >= 0 )
		{
			while (b >= 0)
			{
				if (a>0 && b>0)
				{
					enemies2.push_back(make_unique<SS3>(glm::vec3(a, b, 40.f)));
					enemies2.push_back(make_unique<SS3>(glm::vec3(-a, b, 40.f)));
					enemies2.push_back(make_unique<SS3>(glm::vec3(a, -b, 40.f)));
					enemies2.push_back(make_unique<SS3>(glm::vec3(-a, -b, 40.f)));
				}
				else if (a==0 && b!=0)
				{
					enemies2.push_back(make_unique<SS3>(glm::vec3(a, b, 40.f)));
					enemies2.push_back(make_unique<SS3>(glm::vec3(a, -b, 40.f)));
				}
				else if (b == 0 && a != 0)
				{
					enemies2.push_back(make_unique<SS3>(glm::vec3(a, b, 40.f)));
					enemies2.push_back(make_unique<SS3>(glm::vec3(-a, b, 40.f)));
				}
				else
				{
					enemies2.push_back(make_unique<SS3>(glm::vec3(a, b, 40.f)));
				}

				b = b - 20;
			}			
			a = a - 18;
			b = 40;
		}
	}
	void LoadEnemies3()
	{
		float a = 36;
		float b = 40;

		while (a >= 0) {
			while (b >= 0) {
				if (a != 0) {
					if (b != 0) {
						enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 30.f)));
						enemies3.push_back(make_unique<SS4>(glm::vec3(a, -b, 30.f)));
						enemies3.push_back(make_unique<SS4>(glm::vec3(-a, b, 30.f)));
						enemies3.push_back(make_unique<SS4>(glm::vec3(-a, -b, 30.f)));
					}
					else {
						enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 30.f)));
						enemies3.push_back(make_unique<SS4>(glm::vec3(-a, b, 30.f)));
					}
				}
				else {
					if (b != 0) {
						enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 30.f)));
						enemies3.push_back(make_unique<SS4>(glm::vec3(a, -b, 30.f)));
					}
					else {
						enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 30.f)));
					}
				}

				b = b-10;
			}
			b = 40;
			a = a-12;
		}
		
		/*while (a >= 0)
		{
			while (b >= 0)
			{
				if (a > 0 && b > 0)
				{
					enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 20.f)));
					enemies3.push_back(make_unique<SS4>(glm::vec3(-a, b, 20.f)));
					enemies3.push_back(make_unique<SS4>(glm::vec3(a, -b, 20.f)));
					enemies3.push_back(make_unique<SS4>(glm::vec3(-a, -b, 20.f)));
				}
				else if (a == 0 && b != 0)
				{
					enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 20.f)));
					enemies3.push_back(make_unique<SS4>(glm::vec3(a, -b, 20.f)));
				}
				else if (b == 0 && a != 0)
				{
					enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 20.f)));
					enemies3.push_back(make_unique<SS4>(glm::vec3(-a, b, 20.f)));
				}
				else
				{
					enemies3.push_back(make_unique<SS4>(glm::vec3(a, b, 20.f)));
				}

				b = b-20;
			}
			a = a -4,5;
			b = 40;
		}	*/
	}
	void CreateVAOs()
	{
		shader.reset(new ShaderProgram("Shaders/mvp.vert", "Shaders/fragment.frag"));
		shader->use();
		myShip.CreateVAO();
		map.CreateVAO();
		exp.CreateVAO();
		LoadEnemies1();
		LoadEnemies2();
		LoadEnemies3();
		for (int i = 0; i < enemies1.size(); i++) {
			enemies1.at(i).get()->CreateVAO();
			enemyPos.push_back(enemies1.at(i).get()->Position());
		}
		for (int i = 0; i < enemies2.size(); i++) {			
			enemies2.at(i).get()->CreateVAO();			
			enemyPos.push_back(enemies2.at(i).get()->Position());
		}	
		for (int i = 0; i < enemies3.size(); i++) {
			enemies3.at(i).get()->CreateVAO();
			enemyPos2.push_back(enemies3.at(i).get()->Position());
		}
	};
	void setCamera()
	{
		view = glm::translate(view,myShip.Position()+ glm::vec3(0.0f, 0.0f, -10.f));
		view = glm::rotate(view, glm::radians(183.0f), glm::vec3(1.0f, .0f, 0.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 1.0f, 1200.0f);

		SetView(view);
		SetProjection(projection);
	};
	void Animate(GLFWwindow* Window, float timeDelta,bool str)
	{
		if (str == true)
		{
			Restart();
		}
		if (timeDelta > 0.5)
			return;
		if (KeyboardRestarController(Window) == true)
		{
			Restart();
			std::cout << "Restart!!!";
		}
		if (myShip.ALive() == true)
		{
			myShip.Move(Window);
			ADInt = KeyboardADController(Window);
			if (ADInt == 1)
			{
				myShip.getLaser().setReady(false);
			}
		}
		else
		{
			myShip.SetDirection(glm::vec3(0.0f, 0.0f, 0.f));
			gameStatusDef = true;
		}
	   	  
	   myShip.Animate(timeDelta);

	   myLaserCheckCollision();
	   Enemy1LaserCheckCollision();

	   AnimateCamera(timeDelta);
	   NPSMOVE1(timeDelta);
	   NPSMOVE2(timeDelta);
	   NPSMOVE3(timeDelta);
	   if (gameStatusDef == true)
	   {
		   std::cout << "Defeat!!!"<<endl;
	   }
	   else if (score == 33)
	   {
		   std::cout << "Victory!!!" << endl;
	   }
	};
	void myLaserCheckCollision()
	{
		if (myShip.getLaser().ZMin() >= map.ZMax())
		{
			myShip.getLaser().ReCharge(glm::vec3(myShip.Position().x, myShip.Position().y,myShip.ZMax()));
		}
		for (int i = 0; i < enemies1.size(); i++) {
			if (myShip.getLaser().XMax() >= enemies1.at(i).get()->XMin() && myShip.getLaser().XMin() <= enemies1.at(i).get()->XMax()
				&& myShip.getLaser().YMax() >= enemies1.at(i).get()->YMin() && myShip.getLaser().YMin() <= enemies1.at(i).get()->YMax()
				&& myShip.getLaser().ZMax() >= enemies1.at(i).get()->ZMin() && enemies1.at(i).get()->ALive()==true)
			{
				enemies1.at(i).get()->Dead();
				myShip.getLaser().ReCharge(glm::vec3(myShip.Position().x, myShip.Position().y, myShip.ZMax()));
				score++;
			    cout <<"Score:"<<score<<endl;
				exp.SetPosition(enemies1.at(i).get()->Position());
				exp.Draw(*shader);
			}
		}
		for (int i = 0; i < enemies2.size(); i++) {
			if (myShip.getLaser().XMax() >= enemies2.at(i).get()->XMin() && myShip.getLaser().XMin() <= enemies2.at(i).get()->XMax()
				&& myShip.getLaser().YMax() >= enemies2.at(i).get()->YMin() && myShip.getLaser().YMin() <= enemies2.at(i).get()->YMax()
				&& myShip.getLaser().ZMax() >= enemies2.at(i).get()->ZMin() && enemies2.at(i).get()->ALive() == true)
			{
				enemies2.at(i).get()->Dead();
				myShip.getLaser().ReCharge(glm::vec3(myShip.Position().x, myShip.Position().y, myShip.ZMax()));
				score++;
			    cout << "Score:" << score << endl;
				exp.SetPosition(enemies2.at(i).get()->Position());
				exp.Draw(*shader);
			}
		}
		for (int i = 0; i < enemies3.size(); i++) {
			if (myShip.getLaser().XMax() >= enemies3.at(i).get()->XMin() && myShip.getLaser().XMin() <= enemies3.at(i).get()->XMax()
				&& myShip.getLaser().YMax() >= enemies3.at(i).get()->YMin() && myShip.getLaser().YMin() <= enemies3.at(i).get()->YMax()
				&& myShip.getLaser().ZMax() >= enemies3.at(i).get()->ZMin() && enemies3.at(i).get()->ALive() == true)
			{
				enemies3.at(i).get()->Dead();
				myShip.getLaser().ReCharge(glm::vec3(myShip.Position().x, myShip.Position().y, myShip.ZMax()));
				exp.SetPosition(enemies3.at(i).get()->Position());
				exp.Draw(*shader);
			}
		}
	}
	void Enemy1LaserCheckCollision()
	{
		for (int i = 0; i < enemies1.size(); i++) {
			if (enemies1.at(i).get()->ALive() == true)
			{
				if (enemies1.at(i).get()->getLaser().ZMax() <= map.ZMin())
				{
					enemies1.at(i).get()->getLaser().ReCharge(glm::vec3(enemies1.at(i).get()->Position().x, enemies1.at(i).get()->Position().y, enemies1.at(i).get()->ZMax()));
				}

				if (myShip.getShild().ALive() == true && ADInt == 2 && enemies1.at(i).get()->getLaser().XMax() >= myShip.getShild().XMin() && enemies1.at(i).get()->getLaser().XMin() <= myShip.getShild().XMax()
					&& enemies1.at(i).get()->getLaser().YMax() >= myShip.getShild().YMin() && enemies1.at(i).get()->getLaser().YMin() <= myShip.getShild().YMax()
					&& enemies1.at(i).get()->getLaser().ZMax() <= myShip.getShild().ZMin())
				{
					myShip.getShild().Hit();
					enemies1.at(i).get()->getLaser().ReCharge(glm::vec3(enemies1.at(i).get()->Position().x, enemies1.at(i).get()->Position().y, enemies1.at(i).get()->ZMax()));
				}
				else if (myShip.ALive() == true && enemies1.at(i).get()->getLaser().XMax() >= myShip.XMin() && enemies1.at(i).get()->getLaser().XMin() <= myShip.XMax()
					&& enemies1.at(i).get()->getLaser().YMax() >= myShip.YMin() && enemies1.at(i).get()->getLaser().YMin() <= myShip.YMax()
					&& enemies1.at(i).get()->getLaser().ZMax() <= myShip.ZMin())
				{
					myShip.Dead();
					enemies1.at(i).get()->getLaser().ReCharge(glm::vec3(enemies1.at(i).get()->Position().x, enemies1.at(i).get()->Position().y, enemies1.at(i).get()->ZMax()));
					exp.SetPosition(myShip.Position());
					exp.Draw(*shader);
				}
			}
		}	

	}
	void NPSMOVE1(float timeDelta)
	{
		int random;
		for (int i = 0; i < enemies1.size(); i++)
		{
			
			if (enemies1.at(i).get()->ALive() == true)
			{

			if (enemies1.at(i).get()->Position().x <= 40 && moveX1.at(0) == false && (i == 0 || i == 1))
			{
				enemies1.at(i).get()->Animate(glm::vec3(1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x > 40)
				{
					moveX1.at(0) = true;
				}
			}
			else if (enemies1.at(i).get()->Position().x >= -40 && moveX1.at(0) == true && (i == 0 || i == 1))
			{
				enemies1.at(i).get()->Animate(glm::vec3(-1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x < -40)
				{
					moveX1.at(0) = false;
				}
			}

			if (enemies1.at(i).get()->Position().x <= 40 && moveX1.at(1) == false && (i == 2 || i == 3))
			{
				enemies1.at(i).get()->Animate(glm::vec3(1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x > 40)
				{
					moveX1.at(1) = true;
				}
			}
			else if (enemies1.at(i).get()->Position().x >= -40 && moveX1.at(1) == true && (i == 2 || i == 3))
			{
				enemies1.at(i).get()->Animate(glm::vec3(-1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x < -40)
				{
					moveX1.at(1) = false;
				}
			}

			if (enemies1.at(i).get()->Position().x <= 40 && moveX1.at(2) == false && (i == 4 || i == 5))
			{
				enemies1.at(i).get()->Animate(glm::vec3(1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x > 40)
				{
					moveX1.at(2) = true;
				}
			}
			else if (enemies1.at(i).get()->Position().x >= -40 && moveX1.at(2) == true && (i == 4 || i == 5))
			{
				enemies1.at(i).get()->Animate(glm::vec3(-1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x < -40)
				{
					moveX1.at(2) = false;
				}
			}

			if (enemies1.at(i).get()->Position().x <= 40 && moveX1.at(3) == false && (i == 6 || i == 7))
			{
				enemies1.at(i).get()->Animate(glm::vec3(1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x > 40)
				{
					moveX1.at(3) = true;
				}
			}
			else if (enemies1.at(i).get()->Position().x >= -40 && moveX1.at(3) == true && (i == 6 || i == 7))
			{
				enemies1.at(i).get()->Animate(glm::vec3(-1.f, 0.f, 0.f), timeDelta);
				if (enemies1.at(i).get()->Position().x < -40)
				{
					moveX1.at(3) = false;
				}
			}

			  random = rand() % 100 + 1;

			  if (random % 2 == 0)
			  {
				  enemies1.at(i).get()->getLaser().setReady(false);
			  }
			}
		}
	}
	void NPSMOVE2(float timeDelta)
	{
		SS3 minX = SS3(enemies2.at(0).get()->Position());
		SS3 maxX = SS3(enemies2.at(0).get()->Position());
		for (int i = 0; i < enemies2.size(); i++) {
			if (minX.Position().x> enemies2.at(i).get()->Position().x)
			{
				minX = SS3(enemies2.at(i).get()->Position());
			}
			if (maxX.Position().x < enemies2.at(i).get()->Position().x)
			{
				maxX = SS3(enemies2.at(i).get()->Position());
			}
		}
		for (int i = 0; i < enemies2.size(); i++) {
			if (maxX.Position().x < 40 && moveX2 == false && enemies2.at(i).get()->ALive() == true)
			{
				enemies2.at(i).get()->Animate(glm::vec3(1.f, 0.f, 0.f), timeDelta);
				if (maxX.Position().x >= 39)
				{
					moveX2 = true;
				}
			}
			else if (minX.Position().x >=-40 && moveX2 == true && enemies2.at(i).get()->ALive() == true)
			{
				enemies2.at(i).get()->Animate(glm::vec3(-1.f, 0.f, 0.f), timeDelta);
				if (minX.Position().x < -39)
				{
					moveX2 = false;
				}
			}
		}
	}
	void NPSMOVE3(float timeDelta)
	{
		SS4 minX = SS4(enemies3.at(0).get()->Position());
		SS4 maxX = SS4(enemies3.at(0).get()->Position());
		for (int i = 0; i < enemies3.size(); i++) {
			if (minX.Position().x > enemies3.at(i).get()->Position().x)
			{
				minX = SS4(enemies3.at(i).get()->Position());
			}
			if (maxX.Position().x < enemies3.at(i).get()->Position().x)
			{
				maxX = SS4(enemies3.at(i).get()->Position());
			}
		}
		for (int i = 0; i < enemies3.size(); i++) {
			if (maxX.Position().x < 40 && moveX3 == false && enemies3.at(i).get()->ALive() == true)
			{
				enemies3.at(i).get()->Animate(glm::vec3(1.f, 0.f, 0.f), timeDelta);
				if (maxX.Position().x >= 39)
				{
					moveX3 = true;
				}
			}
			else if (minX.Position().x >= -40 && moveX3 == true && enemies3.at(i).get()->ALive() == true)
			{
				enemies3.at(i).get()->Animate(glm::vec3(-1.f, 0.f, 0.f), timeDelta);
				if (minX.Position().x < -39)
				{
					moveX3 = false;
				}
			}
		}
	}
	void AnimateCamera(float timeDelta)
	{
		view = glm::translate(view, -timeDelta*myShip.Speed()*myShip.Direction());
		SetView(view);	
	};
	void Draw() 
	{	

		if (myShip.ALive() == true)
		{
			if (ADInt == 2)
			{
				myShip.getShild().Draw(*shader);
			}
			myShip.Draw(*shader);
			myShip.getLaser().Draw(*shader);
		}
		
		map.Draw(*shader);

		for (int i = 0; i < enemies1.size(); i++) {
			if (enemies1.at(i).get()->ALive() == true)
			{				
				enemies1.at(i).get()->Draw(*shader);				
				enemies1.at(i).get()->getLaser().Draw(*shader);
			}			
		}
		for (int i = 0; i < enemies2.size(); i++) {
			if (enemies2.at(i).get()->ALive() == true)
			{
				enemies2.at(i).get()->Draw(*shader);
			}		
		}
		for (int i = 0; i < enemies3.size(); i++) {
			if (enemies3.at(i).get()->ALive() == true)
			{
				enemies3.at(i).get()->Draw(*shader);
			}
		}
	};

	void SetView(const glm::mat4& view) { glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view)); }
	void SetProjection(const glm::mat4& proj) { glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj)); }

	~Game()
	{
	};
};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

};
void on_error(int code, const char* text)
{
	std::cout << "Error code: " << code << " Error text: " << text << std::endl;
};


int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

	glfwInit();

	try
	{
		stbi_set_flip_vertically_on_load(true);

		glfwSetErrorCallback(on_error);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", NULL, NULL);
		glfwMakeContextCurrent(window);
		if (window == NULL)
			throw std::exception("Failed to create GLFW window");


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			throw std::exception("Failed to initialize OpenGL context");

		glViewport(0, 0, WIDTH, HEIGHT);
		glEnable(GL_DEPTH_TEST);

		Game Game;
		glfwSetWindowUserPointer(window, (void*)&Game);
		glfwSetKeyCallback(window, key_callback);

		Game.CreateVAOs();
		Game.setCamera();
		int fps=0;
		double newdt = glfwGetTime();
		double olddt = glfwGetTime();
		double frameStart = glfwGetTime();
		double lastFrameStart = glfwGetTime();
		bool str = true;
		while (!glfwWindowShouldClose(window))
		{
			newdt = glfwGetTime();

			frameStart = glfwGetTime();
			float timeDelta = (float)(frameStart - lastFrameStart);

			if (newdt- olddt>=1)
			{
				std::cout <<"FPS:"<< fps << endl;
				olddt = newdt;
				fps = 0;
				str = false;
			}	
			glfwPollEvents();
			KeyboardADController(window);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

			Game.Animate(window, timeDelta, str);;
			Game.Draw();

			glfwSwapBuffers(window);
			fps++;
			lastFrameStart = frameStart;
			
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Unexpected error: " << e.what() << std::endl;
		std::cin.get();
	}

	glfwTerminate();

	return 0;
}

