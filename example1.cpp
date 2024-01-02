//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/

typedef vec4 point4;
typedef vec4 color4;
using namespace std;
// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

#pragma endregion Các biến chức năng
#pragma region 
int on = 0;
GLfloat light_color[] = { 1.0, 1.0, 1.0 };
GLfloat light[3] = { 74, 73, 67 };
color4 light_ambient = color4(1.0, 1.0, 1.0, 1.0);
color4 light_diffuse = color4(1.0, 1.0, 1.0, 1.0);
color4 light_specular = color4(1.0, 1.0, 1.0, 1.0);
point4 light_position = point4(-7, 4.2, 4.6, 1.0);
color4 material_ambient(0.2, 0.2, 0.2, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 1.0, 1.0, 1.0);


float material_shininess = 100;
#pragma endregion Các biến chiếu sáng

vec4 eye(0, 3, -10, 1), at(0, 0, 0, 1), up(0, 1, 0, 1);
GLfloat l = -0.01, r = 0.01, b = -0.01, t = 0.01, zN = 0.02, zF = 50;
mat4 view_mat;
mat4 move_mat;
GLfloat matXY[2]; // doc ngang
GLuint  light_a_loc, light_d_loc, light_s_loc, light_pos_loc, light_shine_loc;
vec4 mat_dinhhuong(0, 0, 1, 1);
GLfloat speedTranslate = 1;
GLfloat speedRotate = 5;
#pragma endregion Các biến điều chỉnh eye


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint model_view_loc, view_loc, projection_loc;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.2, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

}


void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);

	light_a_loc = glGetUniformLocation(program, "AmbientLight");
	light_d_loc = glGetUniformLocation(program, "DiffuseLight");
	light_s_loc = glGetUniformLocation(program, "SpecularLight");
	light_pos_loc = glGetUniformLocation(program, "LightPosition");
	light_shine_loc = glGetUniformLocation(program, "Shininess");
	model_view_loc = glGetUniformLocation(program, "Model_View");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
float Nitpick(int a) {
	return (float)a / 255;
}
color4 ConvertRGB(int a, int b, int c) {
	return color4(Nitpick(a), Nitpick(b), Nitpick(c), 1.0);
}

void FillColor(vec4 color) {
	material_ambient = color;
	material_diffuse = color;
	material_specular = color;
	material_shininess = 100;
	if (on == 1) {
		light[0] = 255;
		light[1] = 255;
		light[2] = 255;
	}
	else if (on == -1) {
		light[0] = 227;
		light[1] = 223;
		light[2] = 163;
	}
	else {
		light[0] = 74;
		light[1] = 73;
		light[2] = 67;
	}
	light_ambient = ConvertRGB(light[0], light[1], light[2]);
	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}

void FillColor2(vec4 diffuse, vec4 ambient, vec4 specular, int dobong) {
	material_ambient = ambient;
	material_diffuse = diffuse;
	material_specular = specular;
	material_shininess = dobong;
	if (on == 1) {
		light[0] = 255;
		light[1] = 255;
		light[2] = 255;
	}
	else if (on == -1) {
		light[0] = 227;
		light[1] = 223;
		light[2] = 163;
	}
	else {
		light[0] = 74;
		light[1] = 73;
		light[2] = 67;
	}
	light_diffuse = ConvertRGB(light[0], light[1], light[2]);
	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
}



mat4 instance;
mat4 model_view, model_cuaso, model_cuaso_trai, model_cuaso_phai, model_canphong;
GLfloat goc = 0;
GLfloat theta[] = { 0, 0, 0 };
GLfloat pha_cuaso = 0;
GLfloat phong[] = { 0, 0, 0 };
GLfloat gocquaycua[] = { 0, 0 };
GLfloat check = 0;
GLfloat quaycuasotrai = 0, quaycuasophai = 0;
//Code cai nha
// 
//Làm khung nhà!!
void Mat_TrenDuoi(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(122, 136, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateX(90) * Scale(40.0, 40.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Mat_TrenDuoi1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(255, 222, 101);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateX(90) * Scale(40.0, 40.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_TraiDoc1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(202, 202, 196);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(10.0, 14.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_TraiDoc2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(202, 202, 196);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(12.0, 14.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_NgangTren(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(202, 202, 196);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(40.0, 3.3, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void Mat_NgangDuoi(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(202, 202, 196);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(40.0, 4.3, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_PhaiDoc1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(202, 202, 196);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(10.0, 14.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_PhaiDoc2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(202, 202, 196);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(26.0, 14.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_SauDoc(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(47, 83, 90);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateY(90) * Scale(8.0, 14.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_SauNgang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(47, 83, 90);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateY(90) * Scale(40.0, 4.0, 1.0);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_canphong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

mat4 model_mattruoc1, model_mattruoc2;

void Mat_Truoc1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(120, 212, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateY(90) * Scale(0.5, 14, 20);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mattruoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Mat_Truoc2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(120, 212, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateY(90) * Scale(0.5, 14, 20);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mattruoc2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat quaycua1;

void Mat_Truoc()
{
	model_mattruoc1 = Translate(-20, 1.3, -22) * RotateY(quaycua1) * Translate(20, -1.3, 22);
	Mat_Truoc1(-10, 1.5, -22);
	model_mattruoc2 = Translate(20, 1.3, -22) * RotateY(-quaycua1) * Translate(-20, -1.3, 22);
	Mat_Truoc2(10, 1.5, -22);
	//model_mattruoc2 = RotateY(quaycua1);
	//Mat_Truoc2(10,1.2,-22);

}

// Làm cửa sổ 

void ThanhNgang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 8);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.0, 0.2, 0.4);
	if (check > 0)
	{
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cuaso_phai * instance);
	}
	else
	{
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cuaso_trai * instance);
	}
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ThanhDoc(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 8);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.2, 6.2, 0.4);
	if (check > 0)
	{
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cuaso_phai * instance);
	}
	else
	{
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cuaso_trai * instance);
	}
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat checkkinh = 0, quaykinhtrai = 0, quaykinhphai = 0;
mat4 model_kinh1_1, model_kinh2_1, model_kinh3_1, model_kinh1_2, model_kinh2_2, model_kinh3_2;
void Kinh(GLfloat x, GLfloat y, GLfloat z)
{
	model_kinh1_1 = model_cuaso_trai;
	model_kinh2_1 = model_cuaso_trai;
	model_kinh3_1 = model_cuaso_trai;
	model_kinh1_2 = model_cuaso_phai;
	model_kinh2_2 = model_cuaso_phai;
	model_kinh3_2 = model_cuaso_phai;

	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.0, 2.0, 0.1);


	if (check < 0)
	{
		if (checkkinh > 0)
		{
			model_kinh3_1 = model_kinh3_1 * Translate(0, 1, 0) * RotateX(quaykinhtrai) * Translate(0, -1, 0);
			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_kinh3_1 * instance);
		}
		else if (checkkinh < 0)
		{
			model_kinh2_1 = model_kinh2_1 * Translate(0, -3, 0) * RotateX(quaykinhtrai) * Translate(0, 3, 0);
			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_kinh2_1 * instance);
		}
		else {
			model_kinh1_1 = model_kinh1_1 * Translate(0, -1, 0) * RotateX(quaykinhtrai) * Translate(0, 1, 0);
			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_kinh1_1 * instance);
		}
	}
	else
	{
		if (checkkinh > 0)
		{
			model_kinh3_2 = model_kinh3_2 * Translate(0, 1, 0) * RotateX(quaykinhphai) * Translate(0, -1, 0);
			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_kinh3_2 * instance);
		}
		else if (checkkinh < 0)
		{
			model_kinh2_2 = model_kinh2_2 * Translate(0, -3, 0) * RotateX(quaykinhphai) * Translate(0, 3, 0);
			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_kinh2_2 * instance);
		}
		else {
			model_kinh1_2 = model_kinh1_2 * Translate(0, -1, 0) * RotateX(quaykinhphai) * Translate(0, 1, 0);
			glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_kinh1_2 * instance);
		}
	}
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

mat4 model_cuasosau_trai, model_cuasosau_phai;
GLfloat checkcuasosau = 0, quaycuasosau1, quaycuasosau2;
void CuaSoSau(GLfloat x, GLfloat y, GLfloat z)
{
	model_cuasosau_trai = model_view;
	model_cuasosau_phai = model_view;

	material_diffuse = ConvertRGB(27, 231, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.4, 6.0, 8);
	if (checkcuasosau > 0)
	{
		model_cuasosau_phai = model_cuasosau_phai * Translate(17.6, -5, -8) * RotateZ(quaycuasosau1) * Translate(-17.6, 5, 8);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cuasosau_phai * instance);

	}
	else
	{
		model_cuasosau_trai = model_cuasosau_trai * Translate(17.6, -5, 8) * RotateZ(quaycuasosau2) * Translate(-17.6, 5, -8);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cuasosau_trai * instance);

	}
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void KhungCuaSo()
{
	ThanhNgang(0, -3, 0);
	ThanhNgang(0, -1, 0);
	ThanhNgang(0, 1, 0);
	ThanhNgang(0, 3, 0);
	ThanhDoc(1.9, 0, 0);
	ThanhDoc(-1.9, 0, 0);
}

void CuaSo(GLfloat x, GLfloat y, GLfloat z)
{
	model_cuaso_trai = model_view;
	model_cuaso_phai = model_view;



	if (check < 0)
	{
		model_cuaso_trai = model_cuaso_trai * Translate(x, y, z) * Translate(0, -3, 0) * RotateX(quaycuasotrai) * Translate(0, 3, 0);
		KhungCuaSo();

		checkkinh = -2;
		Kinh(0, -2, 0);

		checkkinh = 0;
		Kinh(0, 0, 0);

		checkkinh = 2;
		Kinh(0, 2, 0);
	}
	else
	{
		model_cuaso_phai = model_cuaso_phai * Translate(x, y, z) * Translate(0, -3, 0) * RotateX(quaycuasophai) * Translate(0, 3, 0);
		KhungCuaSo();
		model_kinh3_2 = model_kinh3_2 * Translate(0, -1, 0) * RotateX(quaykinhphai) * Translate(0, 1, 0);
		checkkinh = -2;
		Kinh(0, -2, 0);
		model_kinh2_2 = model_kinh2_2 * Translate(0, 1, 0) * RotateX(quaykinhphai) * Translate(0, -1, 0);
		checkkinh = 0;
		Kinh(0, 0, 0);
		model_kinh1_2 = model_kinh1_2 * Translate(0, 3, 0) * RotateX(quaykinhphai) * Translate(0, -3, 0);
		checkkinh = 2;
		Kinh(0, 2, 0);
	}

}

void Can_Phong()
{
	model_canphong = model_view;
	Mat_TrenDuoi(-1.8, 4.5, 0);
	Mat_TrenDuoi1(-1.8, -8.5, 0);
	Mat_TraiDoc1(-3, -2, 20);
	Mat_TraiDoc1(-17, -2, 20);
	Mat_TraiDoc1(-10, -2, 20);
	Mat_TraiDoc2(12, -2, 20);
	Mat_NgangTren(-1, -7.3, 20);
	Mat_NgangDuoi(-2, 2.7, 20);
	Mat_PhaiDoc1(-17, -2, -20);
	Mat_PhaiDoc2(5, -2, -20);
	Mat_NgangTren(-1, -7.3, -20);
	Mat_NgangDuoi(-2, 2.7, -20);
	Mat_SauDoc(17.6, -2, 16);
	Mat_SauDoc(17.6, -2, 0);
	Mat_SauDoc(17.6, -2, -16);
	Mat_SauNgang(17.6, -7, 0);
	Mat_SauNgang(17.6, 3, 0);

	Mat_Truoc();

	checkcuasosau = -8;
	CuaSoSau(17.6, -2, -8);
	checkcuasosau = 8;
	CuaSoSau(17.6, -2, 8);

	check = -10;
	CuaSo(-10, -2.5, -20);
	check = 4.1;
	CuaSo(4.1, -2.5, 20);


}


mat4 model, model_main, model_main1, model_main2, model_main3, model_main4, model_main5, model_nc;

GLfloat xx = 0, yy = 0, zz = 0, a = 0, xt1 = 0, xp1 = 0, xt2 = 0, xp2 = 0, alpha = 0;


//cai giuong
void thangiuong(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(255, 218, 185);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(12, 0.5, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void daugiuong(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(244, 164, 96);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.5, 4.4, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cuoigiuong(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(244, 164, 96);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.5, 2, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void daucuoi()
{
	daugiuong(5.75, 0.48, 0);
	cuoigiuong(-6.2, -0.7, 0);
}

void namtaytrai1(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeotrai1()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeotrai1()
{
	daungankeotrai1();
	namtaytrai1(0, 0.7, -1.5);
}

void namtaytrai2(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeotrai2()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeotrai2()
{
	daungankeotrai2();
	namtaytrai2(0, 0.7, 1.5);
}

void namtayphai1(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main4 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeophai1()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main4 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main4 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main4 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main4 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main4 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeophai1()
{
	daungankeophai1();
	namtayphai1(0, 0.7, 1.5);
}
void namtayphai2(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeophai2()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeophai2()
{
	daungankeophai2();
	namtayphai2(0, 0.7, -1.5);
}

void giuong()
{

	daucuoi();
	thangiuong(0, 0, 0);
}
void giuongfull(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha)
{
	model_main = RotateY(alpha) * Translate(0, -0.3, 0) * Translate(x, y, z) * Translate(xx, yy, zz) * RotateY(a);
	giuong();
	model_main1 = model_main * Translate(0, 0, xt1) * Translate(2.7, -1.57, -1.6);
	ngankeotrai1();
	model_main2 = model_main * Translate(0, 0, xt2) * Translate(2.7, -1.57, 1.6);
	ngankeotrai2();
	model_main3 = model_main * Translate(0, 0, xp1) * Translate(-3.1, -1.57, -1.6);
	ngankeophai2();
	model_main4 = model_main * Translate(0, 0, xp2) * Translate(-3.1, -1.57, 1.6);
	ngankeophai1();
}

//giuong2tang
mat4 model2tang, model_main2tang, model_main12tang, model_main22tang, model_main32tang, model_main42tang, model_main52tang, model_nc2tang;


//cai giuong
void thangiuong2tang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(255, 218, 185);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(12, 0.5, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void daugiuong2tang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(244, 164, 96);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.5, 4.4, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cuoigiuong2tang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(244, 164, 96);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.5, 2, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void daucuoi2tang()
{
	daugiuong2tang(5.75, 0.48, 0);
	cuoigiuong2tang(-6.2, -0.7, 0);
}

void namtaytrai12tang(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main12tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeotrai12tang()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main12tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main12tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main12tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main12tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main12tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeotrai12tang()
{
	daungankeotrai12tang();
	namtaytrai12tang(0, 0.7, -1.5);
}

void namtaytrai22tang(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main22tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeotrai22tang()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main22tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main22tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main22tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main22tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main22tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeotrai22tang()
{
	daungankeotrai22tang();
	namtaytrai22tang(0, 0.7, 1.5);
}

void namtayphai12tang(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeophai12tang()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeophai12tang()
{
	daungankeophai12tang();
	namtayphai12tang(0, 0.7, 1.5);
}
void namtayphai22tang(GLfloat xnk, GLfloat ynk, GLfloat znk)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(xnk, ynk, znk) * Scale(0.7, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main32tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void daungankeophai22tang()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Scale(5.8, 0.3, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main32tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main32tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-2.75, 0.6, 0) * Scale(0.3, 1.5, 3.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main32tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, 1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main32tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0.6, -1.38) * Scale(5.8, 1.5, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main32tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngankeophai22tang()
{
	daungankeophai22tang();
	namtayphai22tang(0, 0.7, -1.5);
}

void nchangiuong2tang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(x, y, z) * Scale(0.5, 6, 0.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanhdoc2tang()
{
	nchangiuong2tang(5.5, 3, 3);
	nchangiuong2tang(-6, 3, 3);
	nchangiuong2tang(5.5, 3, -3);
	nchangiuong2tang(-6, 3, -3);
}

void giuong2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);
	mat4 instance = Translate(x, y, z) * Scale(12, 1, 6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(244, 164, 96);
	FillColor(material_diffuse);

	instance = Translate(x, y, z) * Translate(6, 1, 0) * Scale(1, 3, 6.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(x, y, z) * Translate(2, 3.5, -4.4) * Scale(11, 0.3, 0.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(2, 3.5, 1.2) * Scale(11, 0.3, 0.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_main42tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Translate(-9, -0.2, -0.85) * Scale(1.5, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-9, -0.2, 0.85) * Scale(1.5, 0.3, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanhcauthang2tang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	mat4 instance = Translate(x, y, z) * Scale(0.3, 7, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void baccauthang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	mat4 instance = Translate(x, y, z) * Scale(0.2, 0.2, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model2tang * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cauthang()
{
	thanhcauthang2tang(-10, 2.2, -0.85);
	thanhcauthang2tang(-10, 2.2, 0.85);
	baccauthang(-10, 5, 0);
	baccauthang(-10, 4, 0);
	baccauthang(-10, 3, 0);
	baccauthang(-10, 2, 0);
	baccauthang(-10, 1, 0);
	baccauthang(-10, 0, 0);
}

void thanhbaoho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	mat4 instance = Translate(x, y, z) * Scale(0.3, 2, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_main2tang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void nthanhbh()
{
	thanhbaoho(-5.5, 6.5, -2.8);
	thanhbaoho(-3.5, 6.5, -2.8);
	thanhbaoho(-1.5, 6.5, -2.8);
	thanhbaoho(0.5, 6.5, -2.8);
	thanhbaoho(2.5, 6.5, -2.8);
	thanhbaoho(-5.5, 6.5, 2.8);
	thanhbaoho(-3.5, 6.5, 2.8);
	thanhbaoho(-1.5, 6.5, 2.8);
	thanhbaoho(0.5, 6.5, 2.8);
	thanhbaoho(2.5, 6.5, 2.8);
}

void giuong2tang()
{

	daucuoi2tang();
	thangiuong2tang(0, 0, 0);
	thanhdoc2tang();
	giuong2(0, 5.5, 0);
	nthanhbh();
	cauthang();
}
void giuongfull2(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha)
{
	model_main2tang = RotateY(alpha) * Translate(x, y, z) * Translate(xx, yy, zz) * RotateY(a);
	giuong2tang();
	model_main12tang = model_main2tang * Translate(0, 0, xt1) * Translate(2.7, -1.57, -1.6);
	ngankeotrai12tang();
	model_main22tang = model_main2tang * Translate(0, 0, xt2) * Translate(2.7, -1.57, 1.6);
	ngankeotrai22tang();
	model_main32tang = model_main2tang * Translate(0, 0, xp1) * Translate(-3.1, -1.57, -1.6);
	ngankeophai22tang();
	model_main42tang = model_main2tang * Translate(0, 0, xp2) * Translate(-3.1, -1.57, 1.6);
	ngankeophai12tang();
}


//mo hinh ngoi nha
void tuong()
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);

	mat4 instance = Scale(30, 1, 40);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(255, 245, 238);
	FillColor(material_diffuse);

	instance = Translate(0, 20, 19.5) * Scale(30, 40, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 20, -19.5) * Scale(30, 40, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(205, 133, 63);
	FillColor(material_diffuse);

	instance = Translate(14.5, 20, 0) * Scale(1, 40, 38);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-14.5, 20, 0) * Scale(1, 40, 38);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(240, 255, 255);
	FillColor(material_diffuse);

	instance = Translate(0, 20, -21) * Scale(28, 5, 8);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canhcua(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(139, 69, 19);
	FillColor(material_diffuse);

	mat4 instance = Translate(x, y, z) * Scale(8, 15, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void taycua(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(245, 255, 250);
	FillColor(material_diffuse);

	mat4 instance = Translate(x, y, z) * Scale(1, 0.5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void nhanc()
{
	tuong();
	canhcua(0, 8, -20);
	taycua(-2, 8, -21.5);
}
void nhieunha(GLfloat x, GLfloat y, GLfloat z)
{
	model_nc = Translate(x, y, z);
	nhanc();
}
void nhouse()
{
	nhieunha(-50, -10, -5);
	nhieunha(50, -10, -5);
}

//Bienhieu
void bh(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(245, 255, 250);
	FillColor(material_diffuse);

	mat4 instance = Translate(x, y, z) * Scale(34, 10, 5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chu(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);

	//C
	mat4 instance = Translate(x, y, z) * Translate(15, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(13.5, 24, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(13.5, 20, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//H
	instance = Translate(x, y, z) * Translate(10.5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(8.5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(9.5, 22, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//N
	instance = Translate(x, y, z) * Translate(5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(4, 22, 0) * RotateZ(-25) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(3, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//o
	instance = Translate(x, y, z) * Translate(1.5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(0.5, 24, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(0.5, 20, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-0.5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(0.2, 25.6, 0) * RotateZ(-25) * Scale(0.6, 1.6, 0.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(0.8, 25.6, 0) * RotateZ(25) * Scale(0.6, 1.6, 0.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//i
	instance = Translate(x, y, z) * Translate(-2, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//t
	instance = Translate(x, y, z) * Translate(-5.6, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-5.6, 24, 0) * Scale(3, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//h
	instance = Translate(x, y, z) * Translate(-7.5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-9.5, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-8.5, 22, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//a
	instance = Translate(x, y, z) * Translate(-14, 22, 0) * RotateZ(-22) * Scale(0.9, 5, 0.9);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-12, 22, 0) * RotateZ(22) * Scale(0.9, 5, 0.9);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-13, 22, 0) * Scale(2, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-13.7, 25.6, 0) * RotateZ(-25) * Scale(0.6, 1.6, 0.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-13, 25.6, 0) * RotateZ(25) * Scale(0.6, 1.6, 0.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-13, 27, 0) * RotateZ(25) * Scale(0.6, 1.6, 0.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//t
	instance = Translate(x, y, z) * Translate(-16, 22, 0) * Scale(1, 5, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(x, y, z) * Translate(-16, 24, 0) * Scale(3, 1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

//Ta Thi Tuyet Mai
mat4 model_ghesopha, model_ban1, model_banquay, model_ghe, model_ghexoay, model_QT, model_QTquay;
GLfloat ghe = 0;
GLfloat quattran = 0;
GLfloat ghesopha[] = { 0,0,0 };
GLfloat quaychanban[] = { 0,0 };
GLfloat ban[] = { 0,0,0 };
GLfloat beta[] = { 0,0,0 };
GLfloat beta1[] = { 0,0,0 };

void Thanghesopha(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(7.86, 1.01, 2.25);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Thansaughesopha(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(248, 132, 25);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(7.86, 1.4, 0.52);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tayghesopha(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.8, 0.45, 2.25);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Changhesopha(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.3, 1.8, 0.33);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Ghesopha()
{
	model_ghesopha = model_view * Translate(ghesopha[0], ghesopha[1], ghesopha[2]);
	Thanghesopha(11.05, 2.6, -18.04);
	Thansaughesopha(11.05, 1.54, -18.9);
	Tayghesopha(14.94, 1.89, -18.03);
	Tayghesopha(7.16, 1.89, -18.03);
	Changhesopha(14.94, 3.02, -17.1);
	Changhesopha(14.94, 3.02, -19);
	Changhesopha(7.28, 3.02, -19);
	Changhesopha(7.28, 3.02, -17.1);

}
//Bàn cạnh ghế sô pha 
void Matban(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(255, 255, 255);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.84, 0.38, 3.45);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Chanbandoc(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.25, 2.27, 0.48);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banquay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Chanbanngang(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.25, 0.47, 3.43);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banquay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Chanbantrai() {
	Chanbandoc(13.2, 2.79, -11.15);
	Chanbandoc(13.2, 2.79, -14.07);
	Chanbanngang(13.2, 1.87, -12.61);
	Chanbanngang(13.2, 3.72, -12.61);
}
void Chanbanphai() {
	Chanbandoc(7.57, 2.79, -11.12);
	Chanbandoc(7.6, 2.79, -14.07);
	Chanbanngang(7.6, 1.87, -12.61);
	Chanbanngang(7.6, 3.72, -12.61);
}
void Caiban1()
{
	model_ban1 = model_view * Translate(ban[0], ban[1], ban[2]);
	Matban(10.4, 1.45, -12.60);
	model_banquay = model_ban1 * Translate(13.2, 1.67, -12.61) * RotateZ(quaychanban[0]) * Translate(-13.2, -1.67, 12.61);
	Chanbantrai();
	model_banquay = model_ban1 * Translate(7.6, 1.67, -12.61) * RotateZ(quaychanban[1]) * Translate(-7.6, -1.67, 12.61);
	Chanbanphai();
}

// Cái ghế xoay 
void Trucdung(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(115, 98, 65);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * RotateY(i) * Scale(0.25, 1.63, 0.27);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void Channho(GLfloat x, GLfloat y, GLfloat z, GLfloat t)
{
	material_diffuse = ConvertRGB(115, 98, 65);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateY(t) * Scale(0.13, 0.11, 1.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Changhe() {
	Trucdung(-14.16, 3.04, 16.49);
	Channho(-14.16, 3.8, 16.47, 0);
	Channho(-14.16, 3.8, 16.47, 120);
	Channho(-14.16, 3.8, 16.47, 240);
}
void Mattua(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(255, 255, 255);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 3.41, 0.39);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghexoay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Matngoi(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(244, 164, 96);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(1.8, 0.34, 1.83);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghexoay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Thanhngang(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(115, 98, 65);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * Scale(0.22, 0.2, 1.39);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghexoay * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}
void Thanhdoc(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(115, 98, 65);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateX(11) * Scale(0.22, 1.19, 0.16);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghexoay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Trucnho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(115, 98, 65);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.17, 1.5, 0.19);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghexoay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Thanghe() {
	Mattua(-14.16, 0.32, 17.58);
	Matngoi(-14.17, 1.86, 16.48);
	Thanhngang(-13.18, 0.9, 16.69);
	Thanhdoc(-13.18, 1.42, 16.19);
	Thanhngang(-15.1, 0.9, 16.69);
	Thanhdoc(-15.1, 1.42, 16.19);
	Trucnho(-14.16, 2.69, 16.49);
}
void Caighexoay() {
	model_ghe = model_view * Translate(-14.16, 3.04, 16.49) * Translate(beta[0], beta[1], beta[2]) * Translate(14.16, -3.04, -16.49);
	Changhe();
	model_ghexoay = model_ghe * Translate(-14.17, 1.86, 16.48) * Translate(beta1[0], beta1[1], beta1[2]) * RotateY(ghe) * Translate(14.17, -1.86, -16.48);
	Thanghe();
}

// Quattran
void Tru(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(1, 231, 30);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.84, 0.56, 0.88);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_QT * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tru1(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(255, 255, 255);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * RotateY(i) * Scale(0.19, 1.6, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_QT * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}
void Tru2(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(1, 231, 30);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * RotateY(i) * Scale(0.44, 0.46, 0.44);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_QT * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}
void Truquat() {
	Tru(0, -7.73, 0);
	Tru1(0, -6.9, 0);
	Tru2(0, -6.1, 0);
}
void Canhquat(GLfloat x, GLfloat y, GLfloat z, GLfloat t)
{
	material_diffuse = ConvertRGB(255, 255, 255);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * RotateY(t) * Scale(4.51, 0.05, 0.8);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_QTquay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void CanhQT() {
	Canhquat(0, -6.08, 0, 0);
	Canhquat(0, -6.08, 0, 90);
}
void Quattran(GLfloat x, GLfloat y, GLfloat z) {
	model_QT = model_view * Translate(x, y, z);
	Truquat();
	model_QTquay = model_QT * Translate(0, -6.08, 0) * RotateY(quattran) * Translate(0, 6.08, 0);
	CanhQT();
}
void quayQT_0() {
	quattran = 0;

	glutPostRedisplay();
}
void quayQT_1() {
	quattran += 5;
	if (quattran >= 360) {
		quattran = -360;
	}
	glutPostRedisplay();
}
void quayQT_2() {
	quattran += 10;
	if (quattran >= 360) {
		quattran = -360;
	}
	glutPostRedisplay();
}

//Thêm 2 ghế nhỏ dòng 778-842 ,873,874, 1042-1062 
GLfloat ghenho[] = { 0,0,0 };
GLfloat ghenho1 = 0;
void Thanghenho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2.49, 1.01, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Thansaughenho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(248, 132, 25);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2.5, 1.4, 0.52);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tayghenho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.8, 0.45, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Changhenho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 229, 80);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.3, 1.8, 0.33);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ghesopha * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Ghenho1_1() {
	Thanghenho(8.63, 2.66, -7.79);
	Thansaughenho(8.63, 1.58, -7.04);
	Tayghenho(9.88, 1.95, -7.79);
	Tayghenho(7.38, 1.95, -7.79);
	Changhenho(9.73, 3.1, -6.98);
	Changhenho(9.73, 3.1, -8.62);
	Changhenho(7.55, 3.1, -8.62);
	Changhesopha(7.55, 3.1, -6.98);
}
void Ghenho1()
{
	model_ghesopha = model_view * Translate(8.63, 2.66, -7.79) * Translate(ghenho[0], ghenho[1], ghenho[2]) * RotateY(ghenho1) * Translate(-8.63, -2.66, 7.79);
	Ghenho1_1();

}
void Ghenho2(GLfloat x, GLfloat y, GLfloat z)
{
	model_ghesopha = model_view * Translate(x, y, z);
	Thanghenho(12.69, 2.66, -7.79);
	Thansaughenho(12.69, 1.58, -7.04);
	Tayghenho(13.95, 1.95, -7.79);
	Tayghenho(11.45, 1.95, -7.79);
	Changhenho(13.79, 3.1, -6.98);
	Changhenho(13.79, 3.1, -8.62);
	Changhenho(11.61, 3.1, -8.62);
	Changhesopha(11.61, 3.1, -6.98);

}
//

//ĐIỆP-----------------------------------------------
//cái tủ quần áo
mat4  model_cc, model_cc_2, model_cc_3, model_cc_1;;
void khungtu() {
	material_diffuse = ConvertRGB(137, 43, 49);
	FillColor(material_diffuse);

	//mặt trên
	mat4 instance = Translate(0, 3.9, 0) * Scale(6.4, 0.2, 2.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt dưới 1
	instance = Translate(0, -2, 0) * Scale(6, 0.2, 2.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt dưới 2
	instance = Translate(0, -4, 0) * Scale(6.4, 0.2, 2.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt trái
	instance = Translate(-3.2, 0, 0) * Scale(0.2, 8, 2.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt phải
	instance = Translate(3.2, 0, 0) * Scale(0.2, 8, 2.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt sau
	instance = Translate(0, 0, -1.2) * Scale(6.4, 8, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//thanh treo
	material_diffuse = ConvertRGB(152, 172, 165);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(0, 3, 0) * RotateX(i) * Scale(6.4, 0.1, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}

void canh1() {
	material_diffuse = ConvertRGB(255, 255, 255);
	FillColor(material_diffuse);
	mat4 instance = Translate(-1.58, 3.2, 0) * Scale(3.25, 6, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//núm
	material_diffuse = ConvertRGB(115, 98, 65);
	FillColor(material_diffuse);
	instance = Translate(-2.5, 3.2, 0.15) * Scale(0.15, 1.5, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canh2() {
	material_diffuse = ConvertRGB(255, 255, 255);
	FillColor(material_diffuse);
	mat4 instance = Translate(1.58, 3.2, 0) * Scale(3.25, 6, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(115, 98, 65);
	FillColor(material_diffuse);
	//núm
	instance = Translate(2.5, 3.2, 0.15) * Scale(0.15, 1.5, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void canh3() {
	material_diffuse = ConvertRGB(255, 255, 255);
	FillColor(material_diffuse);
	//mặt trước
	mat4 instance = Translate(0, 1, 1.1) * Scale(6, 1.8, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt sau
	instance = Translate(0, 1, -1) * Scale(6, 1.8, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt trái
	instance = Translate(-3, 1, 0) * Scale(0.2, 1.8, 2.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt phải
	instance = Translate(3, 1, 0) * Scale(0.2, 1.8, 2.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt đáy
	instance = Translate(0, 0.2, 0) * Scale(6, 0.2, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(115, 98, 65);
	FillColor(material_diffuse);
	//núm
	instance = Translate(0, 1.5, 1.25) * Scale(1.5, 0.15, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc_3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
GLfloat zbd, cua1_ad, cua2_ad, z3d;
GLfloat xxd = 0;



void tu() {

	model_cc = RotateY(-90) * Translate(-3, 0, -17) * RotateY(zbd) * Translate(xxd, 0, 0);
	khungtu();


	model_cc_1 = model_cc * Translate(3.2, -2.4, 1.1) * RotateY(cua1_ad);
	canh1();

	model_cc_2 = model_cc * Translate(-3.2, -2.4, 1.1) * RotateY(cua2_ad);
	canh2();

	model_cc_3 = model_cc * Translate(0, -4, 0) * Translate(0, 0, z3d);
	canh3();
}


void tu2() {

	model_cc = RotateY(-90) * Translate(10, 0, -17);
	khungtu();


	model_cc_1 = model_cc * Translate(3.2, -2.4, 1.1);
	canh1();

	model_cc_2 = model_cc * Translate(-3.2, -2.4, 1.1);
	canh2();

	model_cc_3 = model_cc * Translate(0, -4, 0);
	canh3();
}


//Làm bàn tính tiền

mat4 model_bantinhtien, model_ban, model_ngankeo2, model_ngankeo1, model_ngankeo, model_ngankeo3;

void Chan(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.4, 4.0, 0.9);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Chan2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2.8, 4.0, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Chan3(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.1, 4.0, 8.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MatBan(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(66, 231, 218);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(3.5, 0.5, 9.6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


}

void MatBan2(GLfloat x, GLfloat y, GLfloat z)
{


	for (float i = 0; i < 360; i++) {

		material_diffuse = ConvertRGB(231, 231, 231);
		FillColor(material_diffuse);

		instance = Translate(x, y, z) * RotateZ(i) * Scale(0.2, 0.2, 9.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}

}

void MatBan3(GLfloat x, GLfloat y, GLfloat z)
{

	for (float i = 0; i < 360; i++) {

		material_diffuse = ConvertRGB(231, 231, 231);
		FillColor(material_diffuse);

		instance = Translate(x, y, z) * RotateX(i) * Scale(3.2, 0.2, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ban * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}
}


void KhungBan()
{


	Chan(1.35, -2.2, -4.2);
	Chan(-1.35, -2.2, -4.2);
	Chan(1.35, -2.2, 4.2);
	Chan(-1.35, -2.2, 4.2);
	Chan2(0, -2.2, -4.3);
	Chan2(0, -2.2, 4.3);
	Chan3(-1.26, -2.18, 0);
	MatBan(0, 0, 0);
	MatBan2(-1.55, 0.17, 0);
	MatBan2(1.55, 0.17, 0);
	MatBan3(-0.02, 0.17, 4.5);
	MatBan3(-0.02, 0.17, -4.5);


}


void NganKeo2_1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 189);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.02, 0.4, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeo2_2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 189);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2.2, 0.38, 0.02);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeo2()
{

	NganKeo2_1(-3.5, -0.55, 0.5);
	NganKeo2_1(-2.95, -0.55, 0.5);
	NganKeo2_1(-2.4, -0.55, 0.5);
	NganKeo2_1(-1.85, -0.55, 0.5);
	NganKeo2_1(-1.3, -0.55, 0.5);
	NganKeo2_2(-2.4, -0.55, 0);
	NganKeo2_2(-2.4, -0.55, 0.5);
	NganKeo2_2(-2.4, -0.55, 1);

}

void NamNganKeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.2, 0.05, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo3 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeo1_1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.1, 0.5, 1.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeo1_2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2.4, 0.1, 1.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeo1_3(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2.4, 0.5, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void BanPhim(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(1.5, 0.1, 0.9);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Chuot(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(134, 134, 134);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.16, 0.12, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ngankeo1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NganKeo1()
{
	NganKeo1_1(-3.55, -0.55, 0.5);
	NganKeo1_1(-1.25, -0.55, 0.5);
	NganKeo1_2(-2.4, -0.8, 0.5);
	NganKeo1_2(-2.4, -0.3, 0.5);
	NganKeo1_3(-2.4, -0.55, -0.05);

	BanPhim(-2.2, -0.2, 0.5);
	Chuot(-3.4, -0.2, 0.5);
}



//Làm Máy Tính

mat4 model_maytinh, model_maytinh1, model_maytinh2, model_thanhcheo1, model_thanhcheo2, model_thanhcheo3, model_thanhcheo4;



void MT_De(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(134, 134, 134);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 0.1, 0.8);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MT_ThanhDoc(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(134, 134, 134);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * Scale(0.4, 1, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}

void MT_ThanhCheo1(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * Scale(0.2, 1, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_thanhcheo1 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}

void MT_ThanhCheo2(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * Scale(0.2, 0.7, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_thanhcheo2 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}
void MT_ThanhCheo3(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * Scale(0.2, 0.7, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_thanhcheo3 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}
void MT_ThanhCheo4(GLfloat x, GLfloat y, GLfloat z)
{
	for (float i = 0; i < 360; i++) {
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		instance = Translate(x, y, z) * Scale(0.2, 0.7, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_thanhcheo4 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}




void MT1_ManHinh(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(1.94, 1.2, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MT1_1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.03, 1.2, 0.13);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MT1_2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 0.03, 0.13);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MayTinh1()
{
	MT1_ManHinh(0, 0, 0);
	MT1_1(0.98, 0, 0);
	MT1_1(-0.98, 0, 0);
	MT1_2(0, 0.6, 0);
	MT1_2(0, -0.6, 0);

}




void MT2_ManHinh(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(1.94, 1.2, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MT2_1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.03, 1.2, 0.13);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MT2_2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 0.03, 0.13);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_maytinh2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MayTinh2()
{
	MT2_ManHinh(0, 0, 0);
	MT2_1(0.98, 0, 0);
	MT2_1(-0.98, 0, 0);
	MT2_2(0, 0.6, 0);
	MT2_2(0, -0.6, 0);

}


GLfloat model_thanhcheo, thanhcheo1, thanhcheo2;
void ThanhCheo1()
{

	MT_ThanhCheo1(-2.4, 0.4, 1.4);
	MT_ThanhCheo2(-2.4, 0.9, 1.4);
}

void ThanhCheo2()
{


	MT_ThanhCheo3(-2.4, -1.8, 0);
	MT_ThanhCheo4(-2.4, -2.2, 0);
}

void KhungMT()
{
	MT_De(-2.4, 0.3, 1);
	MT_ThanhDoc(-2.4, 0.82, 1);
}

GLfloat son_dichuyenmt[] = { 0,0,0 }, xoaythanhcheo1, checkson;

void MayTinh()
{

	KhungMT();

	model_thanhcheo1 = model_maytinh * RotateX(-45);
	model_thanhcheo2 = model_thanhcheo1 * Translate(0, thanhcheo1, 0);
	ThanhCheo1();

	model_thanhcheo3 = model_maytinh * RotateX(225);
	model_thanhcheo4 = model_thanhcheo3 * Translate(0, thanhcheo2, 0);
	ThanhCheo2();

	model_maytinh1 = model_thanhcheo2 * Translate(-2.2, 1.3, 1.5) * RotateX(33) * RotateX(45);
	MayTinh1();

	model_maytinh2 = model_thanhcheo4 * Translate(-2.2, -2.6, 0) * RotateX(-25) * RotateX(-225);
	MayTinh2();

}

GLfloat son_ngankeo1[] = { 0,0,0 }, son_ngankeo2[] = { 0,0,0 }, son_quaynutngankeo;

void NganKeo()
{

	NganKeo1();
	model_ngankeo2 = model_ngankeo1 * Translate(0, 0, son_ngankeo2[2]);
	NganKeo2();
	model_ngankeo3 = model_ngankeo2 * RotateX(son_quaynutngankeo);
	NamNganKeo(-2.4, -0.85, 1);

}

void CaiBan(GLfloat x, GLfloat y, GLfloat z)
{
	//KhungBan(12.5,0,-16);

	//NganKeo(12.8, 0, -16);

	//MayTinh(10.7, 0, -16);

	model_ban = Translate(x, y, z) * Translate(theta[0], theta[1], theta[2]);
	KhungBan();

	model_ngankeo1 = model_ban * Translate(0.3, 1.1, 0) * RotateY(90) * Translate(son_ngankeo1[0], son_ngankeo1[1], son_ngankeo1[2]);
	NganKeo();

	model_maytinh = model_ban * Translate(-1.8, 0, 0) * RotateY(90) * Translate(son_dichuyenmt[1], son_dichuyenmt[1], son_dichuyenmt[2]);
	MayTinh();
}


//CODE NGOAI CANH

mat4 model_duong;

void MatDuong(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(300, 0.2, 200);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_duong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void VachKe1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(300, 0.1, 5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_duong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void VachKe2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(20, 0.1, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_duong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

mat4 model_denduong;


void DenDuong(GLfloat x, GLfloat y, GLfloat z, GLfloat sxoay)
{
	model_denduong = Translate(x, y, z) * RotateY(sxoay);

	material_diffuse = ConvertRGB(152, 172, 165);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(0, 6.5, -34) * RotateY(i) * Scale(2, 22, 2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_denduong * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(0, 15.8, -34.7) * RotateX(45) * RotateZ(i) * Scale(1, 1, 6);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_denduong * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	instance = Translate(0, 17.7, -38.7) * Scale(4, 0.5, 6);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_denduong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 17.4, -38.7) * Scale(2, 0.5, 4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_denduong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}


void NgoaiCanh(GLfloat x, GLfloat y, GLfloat z)
{
	model_duong = Translate(x, y, z);

	MatDuong(0, -5.3, -22);
	VachKe1(0, -4.5, -35);
	VachKe1(0, -4.5, -95);
	VachKe2(0, -4.5, -65);
	VachKe2(30, -4.5, -65);
	VachKe2(60, -4.5, -65);
	VachKe2(90, -4.5, -65);
	VachKe2(120, -4.5, -65);
	VachKe2(-30, -4.5, -65);
	VachKe2(-60, -4.5, -65);
	VachKe2(-90, -4.5, -65);
	VachKe2(-120, -4.5, -65);

	DenDuong(30, 0, z, 0);
	DenDuong(-30, 0, z, 0);
	DenDuong(90, 0, z, 0);
	DenDuong(-90, 0, z, 0);
	DenDuong(30, 0, -130 + z, 180);
	DenDuong(-30, 0, -130 + z, 180);
	DenDuong(90, 0, -130 + z, 180);
	DenDuong(-90, 0, -130 + z, 180);
}



//code cái xe
mat4 model_xe, model_banh1, model_banh2, model_banh3, model_banh4;
void khungxe() {
	material_diffuse = ConvertRGB(32, 63, 132);
	FillColor(material_diffuse);
	mat4 instance = Scale(9.6, 2, 5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_xe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thungxe() {
	material_diffuse = ConvertRGB(89, 101, 97);
	FillColor(material_diffuse);
	mat4 instance = Translate(1.3, 2.5, 0) * Scale(7, 3, 5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_xe * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void banh1() {

	for (int i = 0; i <= 360; i++)
	{
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		mat4 instance = RotateZ(i) * Scale(1.5, 1.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh1 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

		material_diffuse = ConvertRGB(255, 255, 255);
		FillColor(material_diffuse);
		instance = RotateZ(i) * Scale(1, 0.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh1 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void banh2() {


	for (int i = 0; i <= 360; i++)
	{
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		mat4 instance = RotateZ(i) * Scale(1.5, 1.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh2 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
		material_diffuse = ConvertRGB(255, 255, 255);
		FillColor(material_diffuse);
		instance = RotateZ(i) * Scale(1, 0.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh2 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void banh3() {

	for (int i = 0; i <= 360; i++)
	{
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		mat4 instance = RotateZ(i) * Scale(1.5, 1.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh3 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
		material_diffuse = ConvertRGB(255, 255, 255);
		FillColor(material_diffuse);
		instance = RotateZ(i) * Scale(1, 0.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh3 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void banh4() {

	for (int i = 0; i <= 360; i++)
	{
		material_diffuse = ConvertRGB(0, 0, 0);
		FillColor(material_diffuse);
		mat4 instance = RotateZ(i) * Scale(1.5, 1.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh4 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
		material_diffuse = ConvertRGB(255, 255, 255);
		FillColor(material_diffuse);
		instance = RotateZ(i) * Scale(1, 0.5, 0.5);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banh4 * model_xe * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

GLfloat xxe, b1, b2, b3, b4;
void xe() {
	model_xe = Translate(20, -2, -100) * Translate(xxe, 0, 0);
	//model_xe =  Translate(20, -2, -30);
	khungxe();
	thungxe();

	model_banh1 = model_xe * Translate(-2.5, -1, 2) * RotateZ(b1) * Translate(-20, 2, 100) * Translate(-xxe, 0, 0);
	banh1();

	model_banh2 = model_xe * Translate(2.5, -1, 2) * RotateZ(b1) * Translate(-20, 2, 100) * Translate(-xxe, 0, 0);
	banh2();

	model_banh3 = model_xe * Translate(-2.5, -1, -2) * RotateZ(b1) * Translate(-20, 2, 100) * Translate(-xxe, 0, 0);
	banh3();

	model_banh4 = model_xe * Translate(2.5, -1, -2) * RotateZ(b1) * Translate(-20, 2, 100) * Translate(-xxe, 0, 0);
	banh4();
	/*model_banh2 = model_banh2 * Translate(2.5, -1, 2) * RotateZ(b1);
	banh2();

	model_banh3 = model_banh3 * Translate(-2.5, -1, -2) * RotateZ(b1);
	banh3();

	model_banh4 = model_banh4 * Translate(2.5, -1, -2) * RotateZ(b1);
	banh4();*/
}
void chayxe() {
	xxe -= 0.3;
	if (xxe < -100) xxe = 60;

	b1 += 10;
	glutPostRedisplay();


}

//Cái Điều Hòa

mat4 model_dieuhoa, model_dieuhoa1;

void MatSauDH(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 197, 200);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(7, 2, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dieuhoa * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void MatTrenDH(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 197, 200);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(7, 0.2, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dieuhoa * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MatBenDH(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 197, 200);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.2, 2, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dieuhoa * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MatTruocDH1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(7, 1.4, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dieuhoa * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void MatTruocDH2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(7, 0.6, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dieuhoa1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat quaydh;

void CaiDieuHoa()
{
	model_dieuhoa = Translate(0, 5, 15);
	MatSauDH(0, 1, 2);
	MatTrenDH(0, 2, 1);
	MatTrenDH(0, 0, 1);
	MatBenDH(3.5, 1, 1);
	MatBenDH(-3.4, 1, 1);
	MatTruocDH1(0, 1.4, 0);
	model_dieuhoa1 = model_dieuhoa * RotateX(quaydh);
	MatTruocDH2(0, 0.3, 0);

}


//Lam Ke TV
mat4 model_ketivi;



void BoTron1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateZ(90) * RotateY(i) * Scale(0.2, 6, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void BoTron2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateZ(90) * RotateY(i) * Scale(0.1, 8, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void BoTron3(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateZ(90) * RotateY(i) * Scale(0.1, 2, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void BoTron4(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateZ(90) * RotateY(i) * Scale(0.2, 2, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void BoTron5(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateY(i) * Scale(0.1, 6, 0.1);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void Ngang1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(27, 114, 255);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(8, 0.1, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Ngang2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(6, 0.1, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Ngang3(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 0.1, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Ngang4(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 0.2, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Doc1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(129, 175, 255);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.1, 6, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Doc2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(134, 134, 134);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(2, 6, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Doc3(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(8, 1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Doc4(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.1, 1, 2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_ketivi * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

mat4 naptu1, naptu2;

void NapTu1()
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(3, 0.5, -1) * Scale(2, 1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, naptu1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void NapTu2()
{
	material_diffuse = ConvertRGB(0, 0, 0);
	FillColor(material_diffuse);
	instance = Translate(-1, 0.5, -1) * Scale(2, 1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, naptu2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void Nut1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateX(90) * RotateY(i) * Scale(0.2, 0.2, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, naptu1 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void Nut2(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(x, y, z) * RotateX(90) * RotateY(i) * Scale(0.2, 0.2, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, naptu2 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

mat4 ngankeo;

void NganKeoTV()
{
	material_diffuse = ConvertRGB(0, 0, 231);
	FillColor(material_diffuse);

	instance = Translate(-3, 0.15, -0.15) * Scale(1.8, 0.1, 1.8);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ngankeo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);

	instance = Translate(-2.15, 0.5, -0.1) * Scale(0.1, 0.8, 1.8);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ngankeo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-3.9, 0.5, -0.1) * Scale(0.1, 0.8, 1.8);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ngankeo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(134, 255, 222);
	FillColor(material_diffuse);

	instance = Translate(-3, 0.5, 0.7) * Scale(1.8, 0.8, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ngankeo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-3, 0.5, -1) * Scale(1.8, 0.8, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ngankeo * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	material_diffuse = ConvertRGB(231, 231, 0);
	FillColor(material_diffuse);
	for (int i = 0; i <= 360; i++)
	{
		instance = Translate(-3, 0.466, -1.05) * RotateX(90) * RotateY(i) * Scale(0.2, 0.2, 0.2);
		glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ngankeo * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

GLfloat quaynaptu1, quaynaptu2, diketv[] = { 0,0,0 }, dingankeo[] = { 0, 0, 0 };

void KeTiVi()
{
	model_ketivi = model_ketivi;
	model_ketivi = Translate(-18.5, -4, -16) * RotateY(-90) * Translate(diketv[0], diketv[1], diketv[2]);
	Ngang1(0, 0, 0);
	Ngang1(0, 1, 0);
	Ngang1(0, 4.2, 0);
	Ngang2(1, 1.1, 0);
	Ngang2(1, 4.3, 0);
	Ngang2(1, 6, 0);
	Ngang3(-3, 2.5, 0);
	Ngang4(-3, 6, 0);
	Doc1(-4, 3, 0);
	Doc1(-2, 3, 0);
	Doc2(-3, 3, 1);
	Doc3(0, 0.5, 1);
	Doc4(0, 0.5, 0);
	Doc4(2, 0.5, 0);

	Doc4(4, 0.5, 0);


	naptu1 = model_ketivi * Translate(4, 0.5, -1) * RotateY(quaynaptu1) * Translate(-4, -0.5, 1);
	NapTu1();
	Nut1(2.5, 0.466, -1.05);
	naptu2 = model_ketivi * Translate(-2, 0.5, -1) * RotateY(quaynaptu2) * Translate(2, -0.5, 1);
	NapTu2();
	Nut2(-0.5, 0.466, -1.05);

	BoTron1(1, 1.05, -0.95);
	BoTron1(1, 4.25, -0.95);
	BoTron2(0, 0, -1);
	BoTron2(0, 6, -1);
	BoTron3(-3, 4.2, -1);
	BoTron3(-3, 2.5, -1);
	BoTron4(-3, 6, -0.95);
	BoTron5(-2, 3, -1);
	BoTron5(-4, 3, -1);
	ngankeo = model_ketivi * Translate(0, 0, dingankeo[2]);
	NganKeoTV();
}
//hung
mat4 model_dongho;
//mat4 model_view, model_cuaso;

void matdongho(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 231, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(3.5, 3.5, 0.2);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dongho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cham12(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(138, 48, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dongho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cham3(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(138, 48, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.4, 0.1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dongho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cham6(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(138, 48, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.1, 0.4, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dongho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void cham9(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(138, 48, 231);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.4, 0.1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dongho * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void mat() {
	matdongho(-15.9571, -4.28904, 19.3931);
	cham12(-15.9202, -5.69626, 19.2611);
	cham3(-14.5783, -4.29935, 19.2611);
	cham6(-15.92027, -2.8954, 19.2611);
	cham9(-17.275, -4.29935, 19.2611);
}

mat4 model_quay1;
void kimphut(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(42, 231, 22);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.12, 1.2, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_quay1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void kimgio(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 50, 47);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.17, 0.85, 0.12);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_quay1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat dh[] = { 0.0 };

void dongho(GLfloat x, GLfloat y, GLfloat z) {
	model_dongho = model_view * Translate(x, y, z);
	mat();
	model_quay1 = model_dongho * Translate(-15.9248, -4.22464, 19.2611) * RotateZ(dh[0]) * Translate(15.9248, 4.22464, -19.2611);
	kimphut(-15.9248, -4.82464, 19.2611);
	model_quay1 = model_dongho * Translate(-15.9246, -4.39964, 19.2611) * RotateZ(dh[1]) * Translate(15.9248, 4.39964, -19.2611);
	kimgio(-15.9246, -4.63516, 19.2611);
}
void xoaydongho(int x) {
	dh[0] += 5;
	glutTimerFunc(2000, xoaydongho, 0);

}
void xoaydongho1(int x) {
	dh[1] += 1;
	glutTimerFunc(2000, xoaydongho1, 0);

}


//tủ 1

mat4 model_banhoc;
void matban(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 163, 62);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(11, 0.304, 4.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chantraitren(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.252, 4.07, 0.367);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chantraiduoi(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.252, 4.07, 0.367);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void matsautu(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.72, 4.07, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matdaytu(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.7, 0.15, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matphaitu(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 4.07, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void mattraitu(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 4.07, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matban() {
	matban(-10.1673, 0, 0.614629);
	chantraitren(-5.04732, 2.0728, 2.17863);
	chantraiduoi(-5.04732, 2.0728, -0.898372);
	matsautu(-12.789, 1.94747, -0.946471);
	matdaytu(-12.7886, 3.85633, 0.669429);
	matphaitu(-15.1201, 1.94747, 0.664227);
	mattraitu(-10.5224, 1.94747, 0.664227);
}

mat4 model_quaytu;
void canh(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(154, 167, 95);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.7, 3.72, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_quaytu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void numtu(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 0.45, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_quaytu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canhtu() {
	canh(-12.7886, 2.05182, 2.33083);
	numtu(-10.6686, 1.18386, 2.40363);
}


//mat4 model_ngankeo;
void dayngankeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.22, 0.15, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void saungankeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.22, 1.39, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void truocngankeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.22, 1.39, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void phaingankeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 1.39, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void traingankeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 1.39, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void numngankeo(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.45, 0.15, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void hngankeo() {
	dayngankeo(-7.79282, 1.48536, 0.669429);
	saungankeo(-7.81702, 0.846789, -0.915772);
	truocngankeo(-7.79112, 0.846789, 2.28543);
	phaingankeo(-10.3706, 0.846789, 0.601628);
	traingankeo(-5.26522, 0.846789, 0.601628);
	numngankeo(-7.55412, 0.628751, 2.43883);
}

GLfloat quaytu = 0;
GLfloat quaytu1[] = { 0,0,0 };
GLfloat keongankeo[] = { 0,0,0 };
void banhoc() {
	model_banhoc = model_view * Translate(quaytu1[0], quaytu1[1], quaytu1[2]);
	matban();
	model_quaytu = model_banhoc * Translate(-15.1386, 2.05182, 2.330833) * RotateY(quaytu) * Translate(15.1386, -2.05182, -2.33083);
	canhtu();
	model_banhoc = model_banhoc * Translate(-13.1455, 1.48536, -17.4322) * Translate(keongankeo[0], keongankeo[1], keongankeo[2]) * Translate(13.1455, -1.48536, 17.4322);
	hngankeo();
}

//tủ 2
//mat4 model_ngankeo;
mat4 model_banhoc1;
void matban1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 163, 62);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(11, 0.304, 4.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chantraitren1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.252, 4.07, 0.367);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chantraiduoi1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.252, 4.07, 0.367);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void matsautu1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.72, 4.07, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matdaytu1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.7, 0.15, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matphaitu1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 4.07, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void mattraitu1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(213, 231, 131);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 4.07, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void matban1() {
	matban1(-10.1673, 0, -8.80149);
	chantraitren1(-5.04732, 2.0728, -7.23749);
	chantraiduoi1(-5.04732, 2.0728, -10.3145);
	matsautu1(-12.789, 1.94747, -10.3729);
	matdaytu1(-12.7886, 3.85633, -8.74669);
	matphaitu1(-15.1201, 1.94747, -8.75189);
	mattraitu1(-10.5224, 1.94747, -8.75189);
}
mat4 model_quaytu1;
void canh1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(154, 167, 95);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(4.7, 3.72, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_quaytu1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void numtu1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 0.45, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_quaytu1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhtu1() {
	canh1(-12.7886, 2.05182, -7.08529);
	numtu1(-10.6686, 1.18386, -7.01249);
}

void dayngankeo1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.22, 0.15, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void saungankeo1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.22, 1.39, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void truocngankeo1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(5.22, 1.39, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void phaingankeo1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 1.39, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void traingankeo1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(167, 102, 28);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.15, 1.39, 3.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void numngankeo1(GLfloat x, GLfloat y, GLfloat z)
{
	material_diffuse = ConvertRGB(231, 226, 230);
	FillColor(material_diffuse);
	instance = Translate(x, y, z) * Scale(0.45, 0.15, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_banhoc1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ngankeo1() {
	dayngankeo1(-7.79282, 1.48536, -8.74669);
	saungankeo1(-7.81702, 0.846789, -8.81449);
	truocngankeo1(-7.79112, 0.846789, -7.13069);
	phaingankeo1(-10.3706, 0.846789, -8.81449);
	traingankeo1(-5.26522, 0.846789, -8.81449);
	numngankeo1(-7.55412, 0.628751, -6.97729);
}


GLfloat quaytu1_1 = 0;
GLfloat quaytu2[] = { 0,0,0 };
GLfloat keongankeo1[] = { 0,0,0 };
void banhoc1() {
	model_banhoc1 = model_view * Translate(quaytu2[0], quaytu2[1], quaytu2[2]);
	matban1();
	model_quaytu1 = model_banhoc1 * Translate(-15.1386, 2.05182, 2.330833) * RotateY(quaytu1_1) * Translate(15.1386, -2.05182, -2.33083);
	canhtu1();
	model_banhoc1 = model_banhoc1 * Translate(-7.79282, 1.48536, -8.74669) * Translate(keongankeo1[0], keongankeo1[1], keongankeo1[2]) * Translate(7.79282, -1.48536, 8.74669);
	ngankeo1();
}





void Eye()
{
	// -------------------------Tạo mắt nhìn--------------------------
	vec4 at = eye + normalize(mat_dinhhuong);
	eye.w = at.w = 1;

	mat4 view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	// -------------------------Tạo khung nhìn--------------------------
	mat4 projection = Frustum(l, r, b, t, zN, zF);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Eye();
	model_view = RotateY(90) * RotateZ(180);


	Can_Phong();
	CaiBan(12.5, 0, -16);
	giuongfull(10.7, -2.3, 0.3, -90); //Trang
	giuongfull2(10.7, -2.3, -10, -90);
	nhouse();
	bh(-50, 23.7, 0);
	chu(-50, 0, -10);
	tu();
	tu2();//Điệp
	//Mai
	Ghesopha();
	Caighexoay();
	Quattran(0, 0, 0);
	Caiban1();
	Ghenho1();
	Ghenho2(0, 0, 0);
	//
	NgoaiCanh(0, 0, -10);
	xe();
	CaiDieuHoa();
	KeTiVi();
	//Hung
	dongho(0, 0, 0);
	banhoc();
	banhoc1();
	glutSwapBuffers();
}



void MouseClick(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON: on = 0; glutPostRedisplay(); break;
		case GLUT_MIDDLE_BUTTON: on = 1; glutPostRedisplay(); break;
		case GLUT_RIGHT_BUTTON: on = -1; glutPostRedisplay(); break;
		}
	}
}

bool diepControl = false;
bool trangControl = false;
bool maiControl = false;
bool sonControl = false;
bool hungControl = false;

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler
	if (diepControl == false || trangControl == false || maiControl == false || sonControl == false || hungControl == false) {
		switch (key) {
		case 'D':
			diepControl = true;
			trangControl = false;
			maiControl = false;
			sonControl = false;
			hungControl = false;
			break;
		case 'T':
			diepControl = false;
			trangControl = true;
			maiControl = false;
			sonControl = false;
			hungControl = false;
			break;
		case 'M':
			diepControl = false;
			trangControl = false;
			maiControl = true;
			sonControl = false;
			hungControl = false;
			break;
		case 'S':
			diepControl = false;
			trangControl = false;
			maiControl = false;
			sonControl = true;
			hungControl = false;
			break;
		case 'H':
			diepControl = false;
			trangControl = false;
			maiControl = false;
			sonControl = false;
			hungControl = true;
			break;
			// mở cửa phòng
		case ';':
			quaycua1 += 5;
			if (quaycua1 > 180) quaycua1 = 180;
			break;
			//Đóng cửa phòng
		case ':':
			quaycua1 -= 5;
			if (quaycua1 < 0) quaycua1 = 0;
			break;

			// Chỉnh tọa độ của mắt
		case 'w':
			eye = eye + RotateY(matXY[1]) * vec4(0.0, 0.0, speedTranslate, 1);
			break;
		case 's':
			eye = eye - RotateY(matXY[1]) * vec4(0.0, 0.0, speedTranslate, 1);
			break;
		case 'd':
			eye = eye - RotateY(matXY[1]) * vec4(speedTranslate, 0.0, 0.0, 1);
			break;
		case 'a':
			eye = eye + RotateY(matXY[1]) * vec4(speedTranslate, 0.0, 0.0, 1);
			break;
		case 'q':
			eye = eye - RotateY(matXY[1]) * vec4(0.0, speedTranslate, 0.0, 1);
			break;
		case 'e':
			eye = eye + RotateY(matXY[1]) * vec4(0.0, speedTranslate, 0.0, 1);
			break;
		case 'l':
			matXY[1] -= speedRotate;
			if (matXY[1] >= 360) matXY[1] -= 360;
			if (matXY[1] < 0) matXY[1] += 360;
			break;
		case 'L':
			matXY[1] += speedRotate;
			if (matXY[1] >= 360) matXY[1] -= 360;
			if (matXY[1] < 0) matXY[1] += 360;
			break;
		case 'i':
			matXY[0] -= speedRotate;
			if (matXY[0] < -75) {
				matXY[0] = -75;
			}
			break;
		case 'I':
			matXY[0] += speedRotate;
			if (matXY[0] > 75) {
				matXY[0] = 75;
			}
			break;
		}
	}


	////////////////// SƠNNN ////////////////////////
	if (sonControl == true) {
		switch (key) {
			// Mở cửa sổ phải
		case '1':
			quaycuasotrai -= 5;
			if (quaycuasotrai < -90) quaycuasotrai = -90;
			break;
			// Đóng cửa sổ phải
		case '!':
			quaycuasotrai += 5;
			if (quaycuasotrai > 0) quaycuasotrai = 0;
			break;
			// Mở cửa sổ trái
		case '2':
			quaycuasophai += 5;
			if (quaycuasophai > 90) quaycuasophai = 90;
			break;
			//Đóng cửa sổ trái
		case '@':
			quaycuasophai -= 5;
			if (quaycuasophai < 0) quaycuasophai = 0;
			break;
			//Mở kính phải
		case '3':
			quaykinhtrai += 5;
			if (quaykinhtrai > 30) quaykinhtrai = 30;
			break;
			//Đóng kính phải
		case '#':
			quaykinhtrai -= 5;
			if (quaykinhtrai < 0) quaykinhtrai = 0;

			break;
			// Mở kính trái
		case '4':
			quaykinhphai -= 5;
			if (quaykinhphai < -30) quaykinhphai = -30;
			break;
			// Đóng kính Trái
		case '$':
			quaykinhphai += 5;
			if (quaykinhphai > 0) quaykinhphai = 0;
			break;
			//Bàn Tiến
		case '5':
			theta[0] += 0.2;
			if (theta[0] > 1.2) theta[0] = 1.2;
			break;
			//Bàn lùi
		case '%':
			theta[0] -= 0.2;
			if (theta[0] < -1.2) theta[0] = -1.2;
			break;
			//Bàn sang trái
		case '7':
			theta[2] -= 0.1;
			if (theta[2] < -0.8) theta[2] = -0.8;
			break;
			// Bàn sang phải
		case '&':
			theta[2] += 0.1;
			if (theta[2] > 1) theta[2] = 1;
			break;
			// Ngăn kéo sang trái
		case '8':
			son_ngankeo1[0] += 0.3;
			if (son_ngankeo1[0] > 0.9) son_ngankeo1[0] = 0.9;
			break;
			// Ngăn kéo sang phải
		case '*':
			son_ngankeo1[0] -= 0.1;
			if (son_ngankeo1[0] < -0.3) son_ngankeo1[0] = -0.3;
			break;
			//Ngăn kéo tiến
		case '9':
			son_ngankeo1[2] += 0.1;
			if (son_ngankeo1[2] > 0) son_ngankeo1[2] = 0;
			break;
			//Ngăn kéo lùi
		case '(':
			son_ngankeo1[2] -= 0.1;
			if (son_ngankeo1[2] < -0.5) son_ngankeo1[2] = -0.5;
			break;
			// mở ngăn kéo
		case '0':
			son_ngankeo2[2] += 0.1;
			if (son_ngankeo2[2] > 1) son_ngankeo2[2] = 1;
			break;
			//đóng ngăn kéo
		case ')':
			son_ngankeo2[2] -= 0.1;
			if (son_ngankeo2[2] < 0) son_ngankeo2[2] = 0;
			break;
			// Máy tính phải tiến
		case 'p':
			thanhcheo1 += 0.1;
			if (thanhcheo1 > 0.2) thanhcheo1 = 0.2;
			break;
			// Máy tính phải lùi
		case 'P':
			thanhcheo1 -= 0.1;
			if (thanhcheo1 < -0.2) thanhcheo1 = -0.2;
			break;
			// Máy tính trái tiến
		case '`':
			thanhcheo2 -= 0.1;
			if (thanhcheo2 < -0.2) thanhcheo2 = -0.2;
			break;
			//Máy tính trái lùi
		case '~':
			thanhcheo2 += 0.1;
			if (thanhcheo2 > 0.2) thanhcheo2 = 0.2;
			break;

			//Mở cửa sổ sau trái
		case 'n':
			quaycuasosau1 -= 5;
			if (quaycuasosau1 < -45) quaycuasosau1 = -45;
			break;
			//đóng cửa sổ sau trái
		case 'N':
			quaycuasosau1 += 5;
			if (quaycuasosau1 > 0) quaycuasosau1 = 0;
			break;
			//mở cửa sổ sau phải
		case 't':
			quaycuasosau2 -= 5;
			if (quaycuasosau2 < -45) quaycuasosau2 = -45;
			break;
			//đóng cửa sổ sau phải
		case 'm':
			quaycuasosau2 += 5;
			if (quaycuasosau2 > 0) quaycuasosau2 = 0;
			break;
			//mở điều hòa
		case 'z':
			quaydh -= 5;
			if (quaydh < -125) quaydh = -125;
			break;
			//đóng điểu hòa
		case 'Z':
			quaydh += 5;
			if (quaydh > 0) quaydh = 0;
			break;
			//mơt tủ 1
		case 'y':
			quaynaptu1 -= 5;
			if (quaynaptu1 < -90) quaynaptu1 = -90;
			break;
			//đóng tủ 1
		case 'Y':
			quaynaptu1 += 5;
			if (quaynaptu1 > 0) quaynaptu1 = 0;
			break;
			//mở tủ 2
		case 'o':
			quaynaptu2 += 5;
			if (quaynaptu2 > 90) quaynaptu2 = 90;
			break;
			// đóng tủ 2
		case 'O':
			quaynaptu2 -= 5;
			if (quaynaptu2 < 0) quaynaptu2 = 0;
			break;
			//-----DI CHUYEN KE TI VI -------------
		case 'X':
			diketv[0] += 0.5;
			if (diketv[0] > 5) diketv[0] = 5;
			break;
		case 'x':
			diketv[0] -= 0.5;
			if (diketv[0] < -1) diketv[0] = -1;
			break;
		case 'k':
			diketv[2] += 0.5;
			if (diketv[2] > 0) diketv[2] = 0;
			break;
		case 'K':
			diketv[2] -= 0.5;
			if (diketv[2] < -5) diketv[2] = -5;
			break;
			// MỞ NGĂN KÉO
		case '6':
			dingankeo[2] -= 0.2;
			if (dingankeo[2] < -1.6) dingankeo[2] = -1.6;
			break;
			//ĐÓNG NGĂN KÉO
		case '^':
			dingankeo[2] += 0.2;
			if (dingankeo[2] > 0) dingankeo[2] = 0;
			break;
		}

	}
	if (trangControl == true) {
		switch (key) {
			//// TRANGG //////////////
		case 'x':
			xx = xx + 0.1;
			break;
		case 'X':
			xx = xx - 0.1;
			break;
		case 'z':
			zz = zz + 0.1;
			break;
		case 'Z':
			zz = zz - 0.1;
			break;
		case 'm':
			a = a + 10;
			break;
		case 'M':
			a = a - 10;
			break;
		case 'T':
			xt1 = xt1 + 0.5;
			if (xt1 >= 0) xt1 = 0;
			break;
		case 't':
			xt1 = xt1 - 0.5;
			if (xt1 <= -3.27) xt1 = -3.17;
			break;
		case 'P':
			xp1 = xp1 + 0.5;
			if (xp1 >= 0) xp1 = 0;
			break;
		case 'p':
			xp1 = xp1 - 0.5;
			if (xp1 <= -3.27) xp1 = -3.17;
			break;
		case 'v':
			xt2 = xt2 + 0.5;
			if (xt2 >= 3.27) xt2 = 3.17;
			break;
		case 'V':
			xt2 = xt2 - 0.5;
			if (xt2 <= 0) xt2 = 0;
			break;
		case 'r':
			xp2 = xp2 + 0.5;
			if (xp2 >= 3.27) xp2 = 3.17;
			break;
		case 'R':
			xp2 = xp2 - 0.5;
			if (xp2 <= 0) xp2 = 0;
			break;
		}
	}
	if (diepControl == true) {
		switch (key) {
			//////// ĐIỆP //////////////////
			//tủ
			//quay trái
		case 'q':
			zbd += 5;
			break;
			//quay phải
		case 'Q':
			zbd -= 5;
			break;
			//dịch trái theo Ox
		case 'x':
			xxd += 0.02;
			break;
			//dịch phải theo Õ
		case 'X':
			xxd -= 0.02;
			break;
			//mở cửa trái
		case 'l':
			cua2_ad = cua2_ad - 5;
			if (cua2_ad < -180) cua2_ad = -180;
			break;
			//đóng cửa trái
		case 'L':
			cua2_ad = cua2_ad + 5;
			if (cua2_ad > 0) cua2_ad = 0;
			break;
			//mở cửa phải
		case 'r':
			cua1_ad = cua1_ad + 5;
			if (cua1_ad > 180) cua1_ad = 180;
			break;
			//đóng cửa phải
		case 'R':
			cua1_ad = cua1_ad - 5;
			if (cua1_ad < 0) cua1_ad = 0;
			break;
			//kéo ngăn tủ
		case 'k':
			z3d += 0.1;
			if (z3d > 2.4) z3d = 2.4;
			break;
			//đóng ngắn tủ
		case 'K':
			z3d -= 0.1;
			if (z3d < 0) z3d = 0;
			break;
		}
	}
	/////////mai//////////
	if (maiControl == true)
	{
		switch (key)
		{

			//Gấp chân bàn trái 
		case '[':
			quaychanban[1] -= 5;
			if (quaychanban[1] < -90)
				quaychanban[1] = -90;

			break;
		case ']':
			quaychanban[1] += 5;
			if (quaychanban[1] > 0)
				quaychanban[1] = 0;

			break;
			//Gấp chân bàn phải 
		case '{':
			quaychanban[0] -= 5;
			if (quaychanban[0] < 0)
				quaychanban[0] = 0;

			break;
		case '}':
			quaychanban[0] += 5;
			if (quaychanban[0] > 90)
				quaychanban[0] = 90;

			break;
			//cầm bàn lên
		case 'v':
			ban[1] -= 0.5;
			break;
			//bàn xuống dưới 
		case 'V':
			ban[1] += 0.5;
			break;
			//ban sang phai 
		case 'c':
			ban[0] -= 0.5;
			break;
			//ban sang trai
		case 'C':
			ban[0] += 0.5;
			break;
			//ghe so pha len
		case 'x':
			ghesopha[1] -= 0.5;
			break;
			//ghe so pha xuong
		case 'X':
			ghesopha[1] += 0.5;
			break;
			//ghe so pha sang phải
		case 'z':
			ghesopha[0] -= 0.5;
			break;
			//ghe so pha sang trái 
		case 'Z':
			ghesopha[0] += 0.5;
			break;
			//xoay ghe
		case ',':
			ghe += 5;

			break;
			//ghe sang trai
		case 'B':
			beta[0] -= 0.5;
			break;
			//ghe sang phai
		case 'b':
			beta[0] += 0.5;
			break;
			//ghe len tren 
		case 'y':
			beta[1] -= 0.5;
			break;
			//ghe xuong thap
		case 'Y':
			beta[1] += 0.5;
			break;
			//hạ thấp ghế 
		case 'u':
			beta1[1] += 0.3;
			if (beta1[1] >= -0.3) {
				beta1[1] = -0.3;
			}
			break;
			//Nâng cao ghế
		case 'U':
			beta1[1] -= 0.3;
			if (beta1[1] < -1.2) {
				beta1[1] = -1.2;
			}
			break;
			//bat quat muc 1
		case '1':
			glutIdleFunc(quayQT_1);
			break;
		case '2':
			glutIdleFunc(quayQT_2);
			break;
		case '0':
			glutIdleFunc(quayQT_0);
			break;

			//Thêm 2 ghế nhỏ 
			//ghe nho len
		case 'p':
			ghenho[1] -= 0.5;
			break;
			//ghe nho xuong
		case 'P':
			ghenho[1] += 0.5;
			break;
			//ghe nho sang phải
		case 'n':
			ghenho[0] -= 0.5;
			break;
			//ghe nho sang trái 
		case 'N':
			ghenho[0] += 0.5;
			break;
		case 'o':
			ghenho1 += 5;
			break;
		}

	}
	if (hungControl == true) {
		switch (key) {
			//// Hung //////////////
		case 'x'://dongtu1
			quaytu = quaytu + 5;
			if (quaytu >= 0) {
				quaytu = 0;
			}
			break;
		case 'X'://motu1
			quaytu = quaytu - 5;
			if (quaytu < -90) {
				quaytu = -90;
			}
			break;
		case 'z'://mở ngan keo 1
			keongankeo[2] = keongankeo[2] + 0.2;
			if (keongankeo[2] >= 2) {
				keongankeo[2] = 2;
			}
			break;
		case 'Z'://đóng ngan keo1
			keongankeo[2] = keongankeo[2] - 0.2;
			if (keongankeo[2] <= 0) {
				keongankeo[2] = 0;
			}
			break;
		case 'v'://mở ngăn kéo 2
			keongankeo1[2] = keongankeo1[2] + 0.2;
			if (keongankeo1[2] >= 2) {
				keongankeo1[2] = 2;
			}
			break;
		case 'V':// đóng ngăn kéo 2
			keongankeo1[2] = keongankeo1[2] - 0.2;
			if (keongankeo1[2] <= 0) {
				keongankeo1[2] = 0;
			}
			break;
		case 'b'://tiến tủ 1
			quaytu1[0] = quaytu1[0] + 1;
			break;
		case 'B': //luitu1
			quaytu1[0] = quaytu1[0] - 1;
			break;
		case ':'://sang trai 1
			quaytu1[2] = quaytu1[2] + 0.5;
			break;
		case ';':// sangphai1
			quaytu1[2] = quaytu1[2] - 0.5;
			break;
		case 'g': // tiến tủ 2
			quaytu2[0] = quaytu2[0] + 1;
			break;
		case 'G'://lui2
			quaytu2[0] = quaytu2[0] - 1;
			break;
		case 'n':// trái 2
			quaytu2[2] = quaytu2[2] + 0.5;
			break;
		case 'N'://phải 2
			quaytu2[2] = quaytu2[2] - 0.5;
			break;

		}
	}

	mat_dinhhuong = RotateY(matXY[1]) * RotateX(matXY[0]) * vec4(0, 0, 1, 1);
	mat_dinhhuong.w = 1;


	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	// main function: program starts here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Drawing a Cube");


	glewInit();
	glutIdleFunc(chayxe);
	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(MouseClick);
	glutTimerFunc(2000, xoaydongho, 0);
	glutTimerFunc(2000, xoaydongho1, 0);
	glutMainLoop();
	return 0;
}
