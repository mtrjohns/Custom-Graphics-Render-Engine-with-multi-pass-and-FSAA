

class cRenderClass
{
public:

	cRenderClass(const int, const int);
	~cRenderClass();

	void setViewport2D();

	void setViewport3D();

	void create(int argc, _TCHAR* argv[]);
	void initShaders();
	void loop();

	void render(int);

	// Blinn-Phong Lighting Shader (shader list)
	void BlinnPhong(cShader * pList);

	// Full Screen Antialiasing, Render to Texture
	// (Screen Height, Screen Width, Shader List)
	void FSAA(float screenWidth, float screenHeight, cShader * pList);

	// Calculate Grid Based Intensity, Render To Texture
	// (Grid Size, Screen Height, Screen Width, Shader List)
	void GridBasedIntensity(float gridSize, float screenWidth, float screenHeight, cShader * pList);

	// Full Screen HalfTone Shader, Render To Texture
	// (GridSize, Screen Height, Screen Width, Shader List)
	void HalfTone(float gridSize, float screenWidth, float screenHeight, cShader * pList);

	// Full Screen Intensity Shader, Render to Texture
	// (Intensity(0.0 - 1.0), Screen Height, Screen Width, Shader List)
	void Intensity(float intensity, float screenWidth, float screenHeight, cShader * pList);

	// Draw Triangulated Torus Object
	void DrawTorus();

	void DrawLamp();

	void DrawFly();

	void drawPixel(int, int);	

	inline void clear(){memset( m_buffer, 0, sizeof(float)*m_sw*m_sh*4 );}
	inline void pointSize(int size){m_point_size = size;}
	inline void colour(float r, float g, float b)
	{
		m_colour.r = r;
		m_colour.g = g;
		m_colour.b = b;
	}

	inline void OpenGL2_0(bool value){m_OPENGL_2_0_SUPPORTED = value;}
	inline bool OpenGL2_0(){return m_OPENGL_2_0_SUPPORTED;}
	
	class cShaderInfo	ShaderInfo;

private:
	int		 m_sw, m_sh;
	int		 m_point_size;
	float	*m_buffer;
	sRGB	 m_colour;
	bool	m_OPENGL_2_0_SUPPORTED;
	
	// Setup variables for object and light
	LoadTriangulatedObj *Torus;
	LoadTriangulatedObj *Lamp;
	LoadTriangulatedObj *Fly;
	vec3 lightPos = { 0.0f, 0.0f, 5.0f };
};