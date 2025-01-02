#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Vec3.h"
#include "Scaling.h"
#include "Color.h"


using namespace tinyxml2;
using namespace std;

const int INSIDE = 0;
const int LEFT = 1;   
const int RIGHT = 2;  
const int BOTTOM = 4; 
const int TOP = 8;    




/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
	this->image[i][j].r = c.r;
	this->image[i][j].g = c.g;
	this->image[i][j].b = c.b;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
	string command;

	// TODO: Change implementation if necessary.
	command = "./magick convert " + ppmFileName + " " + ppmFileName + ".png";
	system(command.c_str());
}

/*
	Transformations, clipping, culling, rasterization are done here.
*/



Matrix4 createTranslationMatrix(double tx, double ty, double tz) {
    Matrix4 T;
    T=getIdentityMatrix(); 
    T.values[0][3]= tx;
    T.values[1][3]= ty;
    T.values[2][3]= tz;
    return T;
}

Matrix4 createScalingMatrix(double sx, double sy, double sz) {
    Matrix4 S;
    S=getIdentityMatrix(); 
    S.values[0][0]= sx;
    S.values[1][1]= sy;
    S.values[2][2]= sz;
    return S;
}

Matrix4 createRotationMatrix(double ux, double uy, double uz, double angle) {
    Matrix4 R;
    
    double norm=sqrt(ux * ux + uy * uy + uz * uz);
    ux /=norm;
    uy /=norm;
    uz /=norm;

    double radians= angle * M_PI / 180.0;
    double cosA= cos(radians);
    double sinA= sin(radians);

    R.values[0][0]= cosA + ux * ux * (1 - cosA);
    R.values[0][1]= ux * uy * (1 - cosA) - uz * sinA;
    R.values[0][2]= ux * uz * (1 - cosA) + uy * sinA;

    R.values[1][0]= uy * ux * (1 - cosA) + uz * sinA;
    R.values[1][1]= cosA + uy * uy * (1 - cosA);
    R.values[1][2]= uy * uz * (1 - cosA) - ux * sinA;

    R.values[2][0]=uz * ux * (1 - cosA) - uy * sinA;
    R.values[2][1]=uz * uy * (1 - cosA) + ux * sinA;
    R.values[2][2]= cosA + uz * uz * (1 - cosA);

    R.values[3][3] = 1; 
    return R;
}


Matrix4 createPerspectiveProjectionMatrix(double near, double far, double left, double right, double top, double bottom) {
    Matrix4 Projection;
    Projection.values[0][0] = (2 * near) / (right - left);
    Projection.values[1][1] = (2 * near) / (top - bottom);
    Projection.values[0][2] = (right + left) / (right - left);
    Projection.values[1][2] = (top + bottom) / (top - bottom);
    Projection.values[2][2] = -(far + near) / (far - near);
    Projection.values[2][3] = -(2 * far * near) / (far - near);
    Projection.values[3][2] = -1;
    return Projection;
}


Matrix4 createOrthographicProjectionMatrix(double left, double right, double bottom, double top, double near, double far) {
    Matrix4 Projection;
    Projection.values[0][0] = 2 / (right - left);
    Projection.values[1][1] = 2 / (top - bottom);
    Projection.values[2][2] = -2 / (far - near);
    Projection.values[0][3] = -(right + left) / (right - left);
    Projection.values[1][3] = -(top + bottom) / (top - bottom);
    Projection.values[2][3] = -(far + near) / (far - near);
    Projection.values[3][3] = 1;
    return Projection;
}

double barycentric(double x, double y, double x1, double y1, double x2, double y2){
    return (x * (y1 - y2)) + (y * (x2 - x1)) + (x1 * y2) - (y1 * x2);
}

Vec3 negateVec3(const Vec3 &v) {
    return Vec3(-v.x, -v.y, -v.z);
}



Matrix4 calcModelingTransMatrix(Camera * camera, Mesh & mesh, const vector<Translation*>& translations,const vector<Scaling*>& scalings , const vector<Rotation*>& rotations ) {
 	Matrix4 modelMatrix = getIdentityMatrix();

    
    for (int i=0; i < mesh.numberOfTransformations; ++i) {
        char transType= mesh.transformationTypes[i];
        int transId= mesh.transformationIds[i] - 1; 

        if (transType== 't') {
            const Translation* t = translations[transId];
            Matrix4 T=createTranslationMatrix(t->tx, t->ty, t->tz);
            modelMatrix=multiplyMatrixWithMatrix(T, modelMatrix);
        }
        else if (transType== 's') {
            const Scaling* s = scalings[transId];
            Matrix4 S=createScalingMatrix(s->sx, s->sy, s->sz);
            modelMatrix=multiplyMatrixWithMatrix(S, modelMatrix);
        }
        else if (transType== 'r') {
            const Rotation* r = rotations[transId];
            Matrix4 R=createRotationMatrix(r->ux, r->uy, r->uz, r->angle);
            modelMatrix=multiplyMatrixWithMatrix(R, modelMatrix);
        }
    }

    return modelMatrix;

	}











Matrix4 cameraTransformation(Camera *camera) {
    //Rotation 
    Matrix4 R;
    R.values[0][0] = camera->u.x; R.values[0][1] = camera->u.y; R.values[0][2] = camera->u.z; R.values[0][3] = 0;
    R.values[1][0] = camera->v.x; R.values[1][1] = camera->v.y; R.values[1][2] = camera->v.z; R.values[1][3] = 0;
    R.values[2][0] = camera->w.x; R.values[2][1] = camera->w.y; R.values[2][2] = camera->w.z; R.values[2][3] = 0;
    R.values[3][0] = 0;        R.values[3][1] = 0;        R.values[3][2] = 0;        R.values[3][3] = 1;

    //Translation
    Matrix4 T;
    T.values[0][0] = 1; T.values[0][1] = 0; T.values[0][2] = 0; T.values[0][3] = -camera->position.x;
    T.values[1][0] = 0; T.values[1][1] = 1; T.values[1][2] = 0; T.values[1][3] = -camera->position.y;
    T.values[2][0] = 0; T.values[2][1] = 0; T.values[2][2] = 1; T.values[2][3] = -camera->position.z;
    T.values[3][0] = 0; T.values[3][1] = 0; T.values[3][2] = 0; T.values[3][3] = 1;

    //combined
    return multiplyMatrixWithMatrix(R, T);
}





	
	
	
	Matrix4 calcProjectionTransMatrix(Camera * camera){
	Matrix4 Projection;
    if (camera->projectionType==PERSPECTIVE_PROJECTION) { //Pers
        Projection= createPerspectiveProjectionMatrix(camera->near, camera->far, camera->left, camera->right, camera->top, camera->bottom);} 
	else { //Orth
        Projection = createOrthographicProjectionMatrix(camera->left, camera->right, camera->bottom, camera->top, camera->near, camera->far); }
    return Projection;


	}


Matrix4 calcViewportTransMatrix(Camera * camera) {

		
    Matrix4 V;
    V=getIdentityMatrix();
    V.values[0][0]= camera->horRes/ 2.0;
    V.values[1][1]= camera->verRes/ 2.0;
    V.values[0][3]= (camera->horRes- 1)/ 2.0;
    V.values[1][3]= (camera->verRes- 1)/ 2.0;
    V.values[2][2]= 0.5;
    V.values[2][3]= 0.5;
    return V;
	}
	
	
bool isBackCulled(Vec4 & v1, Vec4 & v2, Vec4 & v3) {
    Vec3 v_1 = Vec3(v1.x, v1.y, v1.z, v1.colorId);
    Vec3 v_2 = Vec3(v2.x, v2.y, v2.z, v2.colorId);
    Vec3 v_3 = Vec3(v3.x, v3.y, v3.z, v3.colorId);
    
    Vec3 edge1 = subtractVec3(v_2, v_1);
    Vec3 edge2 = subtractVec3(v_3, v_1);
    
    Vec3 normalVector = crossProductVec3(edge1, edge2);
    normalVector=normalizeVec3(normalVector);
    double compare = dotProductVec3(normalVector, v_1); 
    
    if (compare<0) {
    return true;
    }
    else{
     return false;
    }
}






bool LiangClip(std::pair<Vec4, Color> & lineStart, std::pair<Vec4, Color> & lineEnd) {
    Vec4 startPoint = lineStart.first, endPoint = lineEnd.first;
    Color startColor = lineStart.second, endColor = lineEnd.second;
    
    double startParam = 0, endParam = 1;
    double deltaX = endPoint.x - startPoint.x, deltaY = endPoint.y - startPoint.y, deltaZ = endPoint.z - startPoint.z;
    Color colorDelta = endColor - startColor;

    double minX = -1, minY = -1, minZ = -1;
    double maxX = 1, maxY = 1, maxZ = 1;

    auto checkVisibility = [&](double denominator, double numerator, double &paramStart, double &paramEnd) -> bool {
        double ratio = numerator / denominator;
        if (denominator > 0) {
            if (ratio > paramEnd)
                return false;
            else if (ratio > paramStart)
                paramStart = ratio;
        } else if (denominator < 0) {
            if (ratio < paramStart)
                return false;
            else if (ratio < paramEnd)
                paramEnd = ratio;
        } else if (numerator > 0) {
            return false;
        }
        return true;
    };

    if (checkVisibility(deltaX, minX - startPoint.x, startParam, endParam) && 
        checkVisibility(-deltaX, startPoint.x - maxX, startParam, endParam) &&
        checkVisibility(deltaY, minY - startPoint.y, startParam, endParam) && 
        checkVisibility(-deltaY, startPoint.y - maxY, startParam, endParam) &&
        checkVisibility(deltaZ, minZ - startPoint.z, startParam, endParam) && 
        checkVisibility(-deltaZ, startPoint.z - maxZ, startParam, endParam)) {

        if (endParam < 1) {
            endPoint = startPoint + Vec4(deltaX * endParam, deltaY * endParam, deltaZ * endParam, 0);
            endColor = startColor + (colorDelta * endParam);
        }
        if (startParam > 0) {
            startPoint = startPoint + Vec4(deltaX * startParam, deltaY * startParam, deltaZ * startParam, 0);
            startColor = startColor + (colorDelta * startParam);
        }
    } else {
        return false;
    }

    lineStart.first = startPoint;
    lineStart.second = startColor;
    lineEnd.first = endPoint;
    lineEnd.second = endColor;

    return true;
}












void rasterizeLine(vector<vector<Color>> & image,vector<vector<double>>& depthBuffer, Vec4 & v0, Vec4 & v1, const Color & c0, const Color & c1) {
    double dx = v1.x - v0.x;
    double dy = v1.y - v0.y;
    double dz = v1.z - v0.z;
    double dColor[3] = {c1.r - c0.r, c1.g - c0.g, c1.b - c0.b};

    //steps for pixels
    int steps = max(abs(dx), abs(dy));
    double xIncrement = dx / (float) steps;
    double yIncrement = dy / (float) steps;
    double zIncrement = dz / (float) steps;
    double colorIncrement[3] = {dColor[0] / steps, dColor[1] / steps, dColor[2] / steps};

    double x = v0.x;
    double y = v0.y;
    double z = v0.z;
    double color[3] = {c0.r, c0.g, c0.b};

    for (int i = 0; i <= steps; i++) {
        int ix = round(x);
        int iy = round(y);

        
        if (ix >= 0 && ix < static_cast<int>(image.size()) && iy >= 0 && iy < static_cast<int>(image[0].size())) {
            if (z < depthBuffer[ix][iy]) {
                image[ix][iy] = Color(color[0], color[1], color[2]);
                depthBuffer[ix][iy] = z;
            }
        }

        x += xIncrement;
        y += yIncrement;
        z += zIncrement;
        color[0] += colorIncrement[0];
        color[1] += colorIncrement[1];
        color[2] += colorIncrement[2];
    }
}






        
        void rasterizeTriangle(vector<vector<Color>>& image, vector<vector<double>>& depthBuffer,  const Color* c0, const Color* c1, const Color* c2, Vec4& v0, Vec4& v1, Vec4& v2, int nx, int ny) {
    
    int x_min = max(static_cast<int>(min({v0.x, v1.x, v2.x})), 0);
    int y_min = max(static_cast<int>(min({v0.y, v1.y, v2.y})), 0);
    int x_max = min(static_cast<int>(max({v0.x, v1.x, v2.x})), nx - 1);
    int y_max = min(static_cast<int>(max({v0.y, v1.y, v2.y})), ny - 1);

    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            double alpha = barycentric(x, y, v1.x, v1.y, v2.x, v2.y) / barycentric(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
            double beta = barycentric(x, y, v2.x, v2.y, v0.x, v0.y) / barycentric(v1.x, v1.y, v2.x, v2.y, v0.x, v0.y);
            double gamma = barycentric(x, y, v0.x, v0.y, v1.x, v1.y) / barycentric(v2.x, v2.y, v0.x, v0.y, v1.x, v1.y);

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                double z = alpha * v0.z + beta * v1.z + gamma * v2.z;
                if (z < depthBuffer[x][y]) {
                    Color c = (*c0 * alpha) + (*c1 * beta) + (*c2 * gamma);
                    image[x][y] = c;
                    depthBuffer[x][y] = z;
                }
            }
        }
    }
}











void Scene::forwardRenderingPipeline(Camera *camera) {
vector<vector<double>> depthBuffer(camera->horRes, vector<double>(camera->verRes, numeric_limits<double>::max()));
Matrix4 M_cam = cameraTransformation(camera);
    Matrix4 M_proj = calcProjectionTransMatrix(camera);
    Matrix4 M_viewport = calcViewportTransMatrix(camera);
    
 

    for (const auto &mesh : this->meshes) {
        Matrix4 M_model = calcModelingTransMatrix(camera, *mesh, this->translations, this->scalings, this->rotations);
        Matrix4 M_cam_model = multiplyMatrixWithMatrix(M_cam, M_model);
        Matrix4 M_proj_cam_model = multiplyMatrixWithMatrix(M_proj, M_cam_model);

        for (const auto &triangle : mesh->triangles) {
            const Vec3 *v0 = this->vertices[triangle.vertexIds[0] - 1];
            const Vec3 *v1 = this->vertices[triangle.vertexIds[1] - 1];
            const Vec3 *v2 = this->vertices[triangle.vertexIds[2] - 1];
            const Color *c0 = this->colorsOfVertices[v0->colorId - 1];
            const Color *c1 = this->colorsOfVertices[v1->colorId - 1];
            const Color *c2 = this->colorsOfVertices[v2->colorId - 1];

            Vec4 projectedV0 = multiplyMatrixWithVec4(M_proj_cam_model, Vec4(v0->x, v0->y, v0->z, 1));
            Vec4 projectedV1 = multiplyMatrixWithVec4(M_proj_cam_model, Vec4(v1->x, v1->y, v1->z, 1));
            Vec4 projectedV2 = multiplyMatrixWithVec4(M_proj_cam_model, Vec4(v2->x, v2->y, v2->z, 1));

            if (this->cullingEnabled && isBackCulled(projectedV0, projectedV1, projectedV2)) {
                continue;
            }

            // Pers
  	    projectedV0 /=projectedV0.t;
            projectedV1 /=projectedV1.t;
            projectedV2 /=projectedV2.t;

            if (mesh->type == WIREFRAME_MESH) {

                std::pair<Vec4, Color> lineStart = {projectedV0, *c0};
                std::pair<Vec4, Color> lineEnd = {projectedV1, *c1};
                if (LiangClip(lineStart, lineEnd)) {
                    Vec4 viewportV0 = multiplyMatrixWithVec4(M_viewport, lineStart.first);
                    Vec4 viewportV1 = multiplyMatrixWithVec4(M_viewport, lineEnd.first);
                    rasterizeLine(this->image, depthBuffer, viewportV0, viewportV1, lineStart.second, lineEnd.second);
                }

                lineStart = {projectedV1, *c1};
                lineEnd = {projectedV2, *c2};
                if (LiangClip(lineStart, lineEnd)) {
                    Vec4 viewportV1 = multiplyMatrixWithVec4(M_viewport, lineStart.first);
                    Vec4 viewportV2 = multiplyMatrixWithVec4(M_viewport, lineEnd.first);
                    rasterizeLine(this->image, depthBuffer, viewportV1, viewportV2, lineStart.second, lineEnd.second);
                }

                lineStart = {projectedV2, *c2};
                lineEnd = {projectedV0, *c0};
                if (LiangClip(lineStart, lineEnd)) {
                    Vec4 viewportV2 = multiplyMatrixWithVec4(M_viewport, lineStart.first);
                    Vec4 viewportV0 = multiplyMatrixWithVec4(M_viewport, lineEnd.first);
                    rasterizeLine(this->image, depthBuffer, viewportV2, viewportV0, lineStart.second, lineEnd.second);
                }
            } else {
                // Solid 
                
                
                Vec4 viewportV0 =multiplyMatrixWithVec4(M_viewport, projectedV0);
                Vec4 viewportV1 =multiplyMatrixWithVec4(M_viewport, projectedV1);
                Vec4 viewportV2 =multiplyMatrixWithVec4(M_viewport, projectedV2);
                rasterizeTriangle(this->image, depthBuffer,c0, c1, c2, viewportV0, viewportV1, viewportV2, camera->horRes, camera->verRes);
            }
        }
    }
}
