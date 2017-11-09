#include "raycast.h"

Pixel* raycast(FILE* fp, int width, int height)
{
	read_file(fp);
	if(objectCount == 0)
	{
		fprintf(stderr, "ERROR: No objects were read in\n");
		exit(0);
	}

	
	// Finished reading in file, now is the time to start Raycasting!

	Pixel* pixMap = (Pixel*) malloc(sizeof(Pixel)*width*height);

	V3 background = malloc(sizeof(double)*3);
	background[0] = backgroundColorR;
	background[1] = backgroundColorG;
	background[2] = backgroundColorB;

	r0 = malloc(sizeof(double)*3);
	r0[0] = (double) cameraX;
	r0[1] = (double) cameraY;
	r0[2] = (double) cameraZ;

	long double pixheight = worldHeight/height; // the height of one pixel
	long double pixwidth = worldWidth/width ; // the width of one pixel

	for(int rowCounter = 0; rowCounter < height; rowCounter++)
	{ // for each row
		double py = cameraY - worldHeight / 2 + pixheight * (rowCounter + 0.5); // y coord of row

		for(int columnCounter = 0; columnCounter < width; columnCounter++)
		{ // for each column
			double px = cameraX - worldWidth / 2 + pixwidth * (columnCounter + 0.5); // x coord of column
			double pz = cameraZ-1; // z coord is on screen
			V3 ur = v3_unit(px,py,pz); // unit ray vector
			int hitObjectIndex = shoot(ur);
			if(hitObjectIndex!=-1)
			{
				pixMap[rowCounter*width+columnCounter].R = objects[hitObjectIndex]->diffuse_color[0]; // return node with the color of what was hit first
				pixMap[rowCounter*width+columnCounter].G = objects[hitObjectIndex]->diffuse_color[1];
				pixMap[rowCounter*width+columnCounter].B = objects[hitObjectIndex]->diffuse_color[2];
			}
			else
			{
				// return the background color since nothing was hit
				pixMap[rowCounter*width+columnCounter].R = backgroundColorR;
				pixMap[rowCounter*width+columnCounter].G = backgroundColorG;
				pixMap[rowCounter*width+columnCounter].B = backgroundColorB;
			}
		}
    }

	return pixMap;
}

// returns the closest object that intersects with the vector
int shoot(V3 rayVector)
{
	int hitObjectIndex = -1;
	double lowestT = -1; // no intersection so far
	// loop through the entire linked list of objects and set t to the closest intersected object
	for(int i = 0; i < objectCount; i++ )
	{
		double result = -1;
		// check if the object intersects with the vector
		if(objects[i]->type == 's') result = ray_sphere_intersection(rayVector,objects[i]);
		else if(objects[i]->type == 'p') result = ray_plane_intersection(rayVector,objects[i]);
		else if(objects[i]->type == 'l') continue;
		else
		{
			fprintf(stderr, "ERROR: Objects can only be type sphere, plane, or light\n");
			exit(0);
		}

		if(result > 0 && (result < lowestT || lowestT == -1))	// this intersection is less than t is already so set t to this result and set hitobject to this object
		{
			lowestT = result;
			hitObjectIndex = i;
		}
	}

	return hitObjectIndex; // should only be a postive number or -1
}

// does the math to calculate a sphere intersection, and if the sphere was intersected then the distance to that sphere
double ray_sphere_intersection(V3 rayVector, Object* obj)
{
	// A = Xd^2 + Yd^2 + Zd^2
	double a = v3_dot(rayVector,rayVector);
	// B = 2 * (Xd * (X0 - Xc) + Yd * (Y0 - Yc) + Zd * (Z0 - Zc))
	double b = 2*v3_dot(rayVector,v3_subtract(r0,obj->position));
	//(X0 - Xc)^2 + (Y0 - Yc)^2 + (Z0 - Zc)^2 - Sr^2
	double c = v3_dot(v3_subtract(r0,obj->position),v3_subtract(r0,obj->position))-pow(obj->radius,2);
	double discriminant = (b*b)-4*a*c;

	// if the discriminant is greater than 0, there was an intersection
	if(discriminant>=0)
	{
		double t0 = (-b-sqrt(discriminant))/(2*a);
		if(t0 > 0) return t0;
		else
		{
			double t1 = (-b+sqrt(discriminant))/(2*a);
			if(t1>0) return t1;
			else return -1;
		}
	}
	else
	{
		return -1;
	}
}

// does the math for a plane intersection and returns the distance to the plane if the intersection happened
double ray_plane_intersection(V3 rayVector, Object* obj)
{
	// -Pn*R0+D
	double num = -1*(v3_dot(obj->normal,r0) + -1*v3_dot(obj->position,obj->normal));
	// Pn*Rd
	double den = v3_dot(obj->normal, rayVector);
	if(den == 0) return -1; // To avoid division by 0
	double t = num/den;
	if(t>0)
	{
		return t;
	}
	else
	{
		return -1;
	}
}

// this function checks if input file exists 
// and returns a 0 if the file exists and a 1 if not
int check_file_path(char* fp)	
{
	FILE *file;
	// if the file can be opened for reading, then it exists
    if ((file = fopen(fp, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

V3 v3_subtract(V3 a, V3 b)
{
	V3 c = malloc(sizeof(double) * 3);

	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];

	return c;
}

V3 v3_scale(V3 a, double b)
{
	V3 c = malloc(sizeof(double) * 3);

	c[0] = a[0] * b;
	c[1] = a[1] * b;
	c[2] = a[2] * b;

	return c;
}

double v3_dot(V3 a, V3 b)
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

V3 v3_assign(double a, double b, double c)
{
	V3 vector = malloc(sizeof(double) * 3);

	vector[0] = a;
	vector[1] = b;
	vector[2] = c;

	return vector;
}

V3 v3_unit(double a, double b, double c)
{
	V3 vector = malloc(sizeof(double) * 3);

	double length = sqrt(a*a+b*b+c*c);

	vector[0] = a/length;
	vector[1] = b/length;
	vector[2] = c/length;

	return vector;
}

void read_file(FILE* fp)
{
	// Read in the first line of the file and make sure it is a camera type object
	// variables to store what is read in from input file
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

	// read in the camera line; assumes camera is the first object in the input file
	if((read = getline(&line, &len, fp)) != -1)
	{
		char object_read_in = '\0';
		int propertyValue = 0;
		char* token = strtok(line," ,:\t");	// this string will be chopped up to get the important information about camera
		while(token != NULL)
		{
			if(strcmp(token,"camera") == 0 && object_read_in == '\0') object_read_in = 'c';	// check that camera is the object for this line
			else if(object_read_in != '\0')
			{
				if(strcmp(token,"width") == 0 && propertyValue == 0) // check if width is the next property for camera and that if a property the propertyValue variable has been reset
				{
					object_read_in = 'w';
					propertyValue = 1;
				}
				else if(strcmp(token,"height") == 0 && propertyValue == 0) // check if the next property is height
				{
					object_read_in = 'h';
					propertyValue = 2;
				}
				else if(strcmp(token,"0") == 0 || atof(token) > 0)
				{
					if(object_read_in == 'w' && propertyValue == 1)
					{
						worldWidth = atof(token);
						propertyValue = 0;
					}
					else if(object_read_in == 'h' && propertyValue == 2) 
					{
						worldHeight = atof(token);
						propertyValue = 0;
					}
					else
					{
						fprintf(stderr, "ERROR: Invalid camera property value.\n");
						exit(0);
					}
				}
				else
				{
					fprintf(stderr, "ERROR: Invalid camera properties or values.\n");
					exit(0);
				}
			}
			else
			{
				fprintf(stderr, "ERROR: First object in input file was %s-- SHOULD BE \'camera\'",token);
				exit(0);
			}
			token = strtok(NULL, ",: \t"); // continue breaking up the read in line by commas

		}
		free(token);
	}
	else
	{
		fprintf(stderr, "ERROR: Empty input file.\n");
	}


	// Store the first object after camera
	objectCount = 0;
	//Object* objects = malloc(sizeof(Object)*129);
	while((read = getline(&line, &len, fp)) != -1)
	{
		char object_read_in = '\0';
		char property_read_in = '\0';
		int propertiesAdded = 0;
		int vectorNum = 3; // set this to three to start with to work with code that will check that this is 3
		Object* object = (Object*) malloc(sizeof(Object));
		if(objectCount != 128)
		{
			char* token = strtok(line," ,\t:[]");
			while(token != NULL)
			{
				//Had to add this because STRTOK makes a token that has a newline character in it and nothing else that matters to the line; so I had to make sure the token got to NULL
				if(count_char_in_string(token,'\n') == 0)
				{
					if(strcmp(token,"\n") != 0)
					{
						// read in an object
						if(object_read_in == '\0')
						{
							if(strcmp(token,"sphere") == 0)
							{
								object->type = 's';
								object_read_in = 's';
							}
							else if(strcmp(token,"plane") == 0)
							{
								object->type = 'p';
								object_read_in = 'p';
							}
							else if(strcmp(token,"light") == 0)
							{
								object->type = 'l';
								object_read_in = 'l';
							}
							else
							{
								fprintf(stderr, "ERROR: Objects can only be type sphere, plane, or light.\n");
								exit(0);
							}
						}
						else
						{
							// property_read_in: c = color, p = position, r = radius, n = normal, f = diffuse color, s = specular color, 
							// t = theta, 0 = radial-a0, 1 = radial-a1, 2 = radial-a2, 3 = angular-a0, d = direction
							if(strcmp(token,"color") == 0 && property_read_in == '\0')
							{
								if(vectorNum == 3) vectorNum = 0;
								else
								{
									fprintf(stderr, "ERROR: Vectors should include 3 numbers in brackets\n");
									exit(0);
								}
								property_read_in = 'c';
								propertiesAdded++;
							}
							else if(strcmp(token,"position") == 0 && property_read_in == '\0')
							{
								if(vectorNum == 3) vectorNum = 0;
								else
								{
									fprintf(stderr, "ERROR: Vectors should include 3 numbers in brackets\n");
									exit(0);
								}
								property_read_in = 'p';
								propertiesAdded++;
							}
							else if(strcmp(token,"radius") == 0 && object_read_in == 's' && property_read_in == '\0')
							{
								property_read_in = 'r';
								propertiesAdded++;
							}
							else if(strcmp(token,"normal") == 0 && object_read_in == 'p' && property_read_in == '\0')
							{
								if(vectorNum == 3) vectorNum = 0;
								else
								{
									fprintf(stderr, "ERROR: Vectors should include 3 numbers in brackets\n");
									exit(0);
								}
								property_read_in = 'n';
								propertiesAdded++;
							}
							else if(strcmp(token,"diffuse_color") == 0 && property_read_in == '\0' && object_read_in != 'l')
							{
								if(vectorNum == 3) vectorNum = 0;
								else
								{
									fprintf(stderr, "ERROR: Vectors should include 3 numbers in brackets\n");
									exit(0);
								}
								property_read_in = 'f';
								propertiesAdded++;
							}
							else if(strcmp(token,"specular_color") == 0 && property_read_in == '\0' && object_read_in != 'l')
							{
								if(vectorNum == 3) vectorNum = 0;
								else
								{
									fprintf(stderr, "ERROR: Vectors should include 3 numbers in brackets\n");
									exit(0);
								}
								property_read_in = 's';
								propertiesAdded++;
							}
							else if(strcmp(token,"theta") == 0 && object_read_in == 'l' && property_read_in == '\0')
							{
								property_read_in = 't';
								propertiesAdded++;
							}
							else if(strcmp(token,"radial-a0") == 0 && object_read_in == 'l' && property_read_in == '\0')
							{
								property_read_in = '0';
								propertiesAdded++;
							}
							else if(strcmp(token,"radial-a1") == 0 && object_read_in == 'l' && property_read_in == '\0')
							{
								property_read_in = '1';
								propertiesAdded++;
							}
							else if(strcmp(token,"radial-a2") == 0 && object_read_in == 'l' && property_read_in == '\0')
							{
								property_read_in = '2';
								propertiesAdded++;
							}
							else if(strcmp(token,"angular-a0") == 0 && object_read_in == 'l' && property_read_in == '\0')
							{
								property_read_in = '3';
								propertiesAdded++;
							}
							else if(strcmp(token,"direction") == 0 && object_read_in == 'l' && property_read_in == '\0')
							{
								if(vectorNum == 3) vectorNum = 0;
								else
								{
									fprintf(stderr, "ERROR: Vectors should include 3 numbers in brackets\n");
									exit(0);
								}
								property_read_in = 'd';
								propertiesAdded++;
							}	
							else if(property_read_in == 'c')
							{
								if(vectorNum >= 3)
								{
									fprintf(stderr, "ERROR: Pixels can only have 3 channels.\n");
									exit(0);
								}
								else
								{
									// malloc the space to add the pixel if nothing has been stored yet TODO: check if space was malloced???
									if(vectorNum == 0) object->pix = malloc(sizeof(double)*3);
									if(vectorNum == 2) property_read_in = '\0';
									// check that the value is valid and store it if it is
									if(strcmp(token,"0") == 0 || atof(token) > 0) object->pix[vectorNum++] = atof(token);
									else
									{
										fprintf(stderr, "ERROR: Values for the Color Property must be positive numbers.\n");
										exit(0);
									}
								}
							}
							else if(property_read_in == 'p')
							{
								if(vectorNum >= 3)
								{
									fprintf(stderr, "ERROR: Position Property can only have 3 coordinates.\n");
									exit(0);
								}
								else
								{
									// malloc the space to add the pixel if nothing has been stored yet TODO: check if space was malloced???
									if(vectorNum == 0) object->position = malloc(sizeof(double)*3);
									if(vectorNum == 2) property_read_in = '\0';
									// check that the value is valid and store it if it is
									if(strcmp(token,"0") == 0 || atof(token) != 0) object->position[vectorNum++] = atof(token);
									else
									{
										fprintf(stderr, "ERROR: Values for the Position Property must be numbers.\n");
										exit(0);
									}
								}

							}
							else if(property_read_in == 'n')
							{
								if(vectorNum >= 3)
								{
									fprintf(stderr, "ERROR: Normal property can only have 3 coordinates.\n");
									exit(0);
								}
								else
								{
									// malloc the space to add the pixel if nothing has been stored yet TODO: check if space was malloced???
									if(vectorNum == 0) object->normal = malloc(sizeof(double)*3);
									if(vectorNum == 2) property_read_in = '\0';
									// check that the value is valid and store it if it is
									if(strcmp(token,"0") == 0 || atof(token) != 0) object->normal[vectorNum++] = atof(token);
									else
									{
										fprintf(stderr, "ERROR: Values for the Normal Property must be numbers.");
										exit(0);
									}
								}

							}
							else if(property_read_in == 'r')
							{
								if(strcmp(token,"0") == 0 || atof(token) > 0) object->radius = atof(token);
								else
								{
									fprintf(stderr, "ERROR: Values for the Radius Property must be positive numbers.\n");
									exit(0);
								}
								property_read_in = '\0';
							}
							else if(property_read_in == 'f')
							{
								if(vectorNum >= 3)
								{
									fprintf(stderr, "ERROR: Diffuse color can only have 3 channels.\n");
									exit(0);
								}
								else
								{
									// malloc the space to add the pixel if nothing has been stored yet TODO: check if space was malloced???
									if(vectorNum == 0) object->diffuse_color = malloc(sizeof(double)*3);
									if(vectorNum == 2) property_read_in = '\0';
									// check that the value is valid and store it if it is
									if(strcmp(token,"0") == 0 || atof(token) > 0) object->diffuse_color[vectorNum++] = atof(token);
									else
									{
										fprintf(stderr, "ERROR: Values for the Diffuse Color Property must be positive numbers.\n");
										exit(0);
									}
								}
							}
							else if(property_read_in == 's')
							{
								if(vectorNum >= 3)
								{
									fprintf(stderr, "ERROR: Specular color can only have 3 channels.\n");
									exit(0);
								}
								else
								{
									// malloc the space to add the pixel if nothing has been stored yet TODO: check if space was malloced???
									if(vectorNum == 0) object->specular_color = malloc(sizeof(double)*3);
									if(vectorNum == 2) property_read_in = '\0';
									// check that the value is valid and store it if it is
									if(strcmp(token,"0") == 0 || atof(token) > 0) object->specular_color[vectorNum++] = atof(token);
									else
									{
										fprintf(stderr, "ERROR: Values for the Specular Color Property must be positive numbers.\n");
										exit(0);
									}	
							}
							}
							else if(property_read_in == 't')
							{
								if(strcmp(token,"0") == 0 || atof(token) != 0) object->theta = atof(token);
								else
								{
									fprintf(stderr, "ERROR: Values for the Theta Property must be numbers.\n");
									exit(0);
								}
								property_read_in = '\0';
							}
							else if(property_read_in == '0')
							{
								if(strcmp(token,"0") == 0 || atof(token) != 0) object->radialA0 = atof(token);
								else
								{
									fprintf(stderr, "ERROR: Values for the Radial-a0 Property must be numbers.\n");
									exit(0);
								}
								property_read_in = '\0';
							}
							else if(property_read_in == '1')
							{
								if(strcmp(token,"0") == 0 || atof(token) != 0) object->radialA1 = atof(token);
								else
								{
									fprintf(stderr, "ERROR: Values for the Radial-a1 Property must be numbers.\n");
									exit(0);
								}
								property_read_in = '\0';
							}
							else if(property_read_in == '2')
							{
								if(strcmp(token,"0") == 0 || atof(token) != 0) object->radialA2 = atof(token);
								else
								{
									fprintf(stderr, "ERROR: Values for the Radial-a2 Property must be numbers.\n");
									exit(0);
								}
								property_read_in = '\0';
							}
							else if(property_read_in == '3')
							{
								if(strcmp(token,"0") == 0 || atof(token) != 0) object->angularA0 = atof(token);
								else
								{
									fprintf(stderr, "ERROR: Values for the Angular-a0 Property must be numbers.\n");
									exit(0);
								}
								property_read_in = '\0';
							}
							else if(property_read_in == 'd')
							{
								if(vectorNum >= 3)
								{
									fprintf(stderr, "ERROR: Direction property can only have 3 coordinates.\n");
									exit(0);
								}
								else
								{
									// malloc the space to add the pixel if nothing has been stored yet TODO: check if space was malloced???
									if(vectorNum == 0) object->direction = malloc(sizeof(double)*3);
									if(vectorNum == 2) property_read_in = '\0';
									// check that the value is valid and store it if it is
									if(strcmp(token,"0") == 0 || atof(token) != 0) object->direction[vectorNum++] = atof(token);
									else
									{
										fprintf(stderr, "ERROR: Values for the Direction Property must be numbers.");
										exit(0);
									}
								}
							}
							else
							{
								fprintf(stderr, "ERROR: Invalid value or property\n");
								exit(0);
							}
						}
					}
				}
				token = strtok(NULL," ,\t:[]");
			}

			if(propertiesAdded < 3)
			{
				fprintf(stderr, "ERROR: At least three properties should have been read in -- NOT %d\n", propertiesAdded);
				exit(0);
			}
			else if(propertiesAdded > 5 && object->type != 'l')
			{
				fprintf(stderr, "ERROR: No more than five properties should have been read in for sphere or plane object\n");
				exit(0);
			}
			else if(propertiesAdded > 6 && object->type == 'l')
			{
				fprintf(stderr, "ERROR: No more than five properties should have been read in for sphere or plane object\n");
				exit(0);
			}
			objects[objectCount++] = object;
		}
		else
		{
			fprintf(stderr, "ERROR: More than 128 objects in input file.\n");
			exit(0);
		}
	}
}

int count_char_in_string(char* inString, char charToCount)
{
	// counts how many instances of charToCount exist in inString
	// returns that count
	int instancesOfChar = 0;
	int charInString = 0;
	while(inString[charInString]!='\0')
	{
		if (inString[charInString] == charToCount)
		{
			instancesOfChar++;
		}
		charInString++;
	}
	return instancesOfChar;
}
