/*
 Ryan Thring, Student ID: 1058718
 Some code used from http://www.xmlsoft.org/html/index.html and
  http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 */

#include "SVGParser.h"
#include <math.h>

#define PI 3.14159265359

/*
// Checks if a string has any uppercase letters, returns 0 if it does and 1 otherwise
static int allLowerCase(char* str) {
  int i = 0;
  if (str == NULL) {
    return 0;
  }
  while (str[i] != '\0') {
    if ((str[i] <= 'Z') && (str[i] >= 'A')) {
      return 0;
    }
    i++;
  }
  return 1;
}
*/

// Compare 2 attributes based on the first char of their name.
int compareAttributes(const void* first,const void* second) {
  return (((Attribute *) first)->name)[0] - (((Attribute *) second)->name)[0];
}

// Compare 2 groups based on the amount of other attributes they have.
int compareGroups(const void* first,const void* second) {
  return getLength(((Group *)first)->otherAttributes) - getLength(((Group *)second)->otherAttributes);
}

// Compare 2 rectangles based on their x positions.
int compareRectangles(const void* first,const void* second) {
  return ((Rectangle *)first)->x - ((Rectangle *)second)->x;
}

// Compare 2 circles based on their x position.
int compareCircles(const void* first,const void* second) {
  return ((Circle *)first)->cx - ((Circle *)second)->cx;
}

// Compare 2 paths based on the first letter of their data.
int comparePaths(const void* first,const void* second) {
  return (((Path *) first)->data)[0] - (((Path *) second)->data)[0];
}

// Convert an attribute to a readable string displaying its properties.
char* attributeToString (void* data) {
  char* the_string;
  int length;
  if (data == NULL) {
    return NULL;
  }
  Attribute* attr = (Attribute *) data;
  length = 27 + strlen(attr->name) + strlen(attr->value);
  the_string = malloc(length);
  strcpy(the_string, "Attribute name: ");
  strcat(the_string, attr->name);
  strcat(the_string, ", value: ");
  strcat(the_string, attr->value);
  strcat(the_string, "\n");
  return the_string;
}

// Convert a group to a readable string displaying its properties.
char* groupToString (void* data) {
  char* the_string;
  char* rects_str;
  char* circs_str;
  char* paths_str;
  char* grps_str;
  char* attr_str;
  if (data == NULL) {
    return NULL;
  }
  Group* grp = (Group *) data;
  rects_str = toString(grp->rectangles);
  circs_str = toString(grp->circles);
  paths_str = toString(grp->paths);
  grps_str = toString(grp->groups);
  attr_str = toString(grp->otherAttributes);
  the_string = malloc(253 + strlen(rects_str) + strlen(circs_str) + strlen(paths_str) + strlen(grps_str) + strlen(attr_str)); //239
  strcpy(the_string, "This is a group. Here are its rectangles: \n"); // 42
  strcat(the_string, rects_str);
  strcat(the_string, "These are the group's circles: \n"); // 31
  strcat(the_string, circs_str);
  strcat(the_string, "These are the group's paths: \n"); // 29
  strcat(the_string, paths_str);
  strcat(the_string, "These are the group's groups: \n"); // 30
  strcat(the_string, grps_str);
  strcat(the_string, "These are the group's other attributes: \n"); // 40
  strcat(the_string, attr_str);
  strcat(the_string, "This is the end of the most recently started group that has not ended yet.\n"); // 74
  free(rects_str);
  free(circs_str);
  free(paths_str);
  free(grps_str);
  free(attr_str);
  return the_string;
}

// Convert a rectangle to a readable string displaying its properties.
char* rectangleToString (void* data) {
  char* the_string;
  char* attr_str;
  char buffer[200];
  if (data == NULL) {
    return NULL;
  }
  Rectangle* rect = (Rectangle *) data;
  sprintf(buffer, "Rectangle positioned at (%f, %f), with a width of %f and a height of %f. These numbers were measured in: %s\n", rect->x, rect->y, rect->width, rect->height, rect->units);
  attr_str = toString(rect->otherAttributes);
  the_string = malloc(strlen(buffer) + strlen(attr_str) + 18);
  strcpy(the_string, buffer);
  strcat(the_string, attr_str);
  strcat(the_string, "End of rectangle\n");
  free(attr_str);
  return the_string;
}

// Convert a circle to a readable string displaying its properties.
char* circleToString (void* data) {
  char* the_string;
  char* attr_str;
  char buffer[200];
  if (data == NULL) {
    return NULL;
  }
  Circle* circ = (Circle *) data;
  sprintf(buffer, "Circle positioned at (%f, %f), with a radius of %f. These numbers were measured in: %s\n", circ->cx, circ->cy, circ->r, circ->units);
  attr_str = toString(circ->otherAttributes);
  the_string = malloc(strlen(buffer) + strlen(attr_str) + 15);
  strcpy(the_string, buffer);
  strcat(the_string, attr_str);
  strcat(the_string, "End of circle\n");
  free(attr_str);
  return the_string;
}

// Convert a path to a readable string displaying its properties.
char* pathToString (void* data) {
  char* the_string;
  char* attr_str;
  if (data == NULL) {
    return NULL;
  }
  Path* the_path = (Path *) data;
  attr_str = toString(the_path->otherAttributes);
  the_string = malloc(30 + strlen(attr_str) + strlen(the_path->data)); //29
  strcpy(the_string, "Path with data: ");
  strcat(the_string, the_path->data);
  strcat(the_string, "\n");
  strcat(the_string, attr_str);
  strcat(the_string, "End of path\n");
  free(attr_str);
  return the_string;
}

// Free all space malloced by the given attribute.
void deleteAttribute (void* data) {
  if (data == NULL) {
    return;
  }
  free(((Attribute *) data)->name);
  free(((Attribute *) data)->value);
  free(data);
}

// Free all space malloced by the given group.
void deleteGroup (void* data) {
  if (data == NULL) {
    return;
  }
  Group* the_group = (Group *) data;
  freeList(the_group->rectangles);
  freeList(the_group->circles);
  freeList(the_group->paths);
  freeList(the_group->groups);
  freeList(the_group->otherAttributes);
  free(the_group);
}

// Free all space malloced by the given rectangle.
void deleteRectangle (void* data) {
  if (data == NULL) {
    return;
  }
  freeList(((Rectangle *) data)->otherAttributes);
  free(data);
}

// Free all space malloced by the given circle.
void deleteCircle (void* data) {
  if (data == NULL) {
    return;
  }
  freeList(((Circle *) data)->otherAttributes);
  free(data);
}

// Free all space malloced by the given path.
void deletePath (void* data) {
  if (data == NULL) {
    return;
  }
  free(((Path *) data)->data);
  freeList(((Path *) data)->otherAttributes);
  free(data);
}

// Return without doing anything.
// Used in lists that should not be freeing the original data when freed.
void fakeDelete (void* data) {
  return;
}

// Truncate the given string to the given size.
static void stringTruncate(char* string, int size) {
  if (strlen(string) >= size) {
    *(string + size - 1) = '\0';
  }
}

// Create an attriute with the given name and contents.
static Attribute* createOtherAttribute(char* attr_name, char* cont) {
  Attribute* shape_attr = malloc(sizeof(Attribute));
  shape_attr->name = malloc(strlen(attr_name) + 1);
  strcpy(shape_attr->name, attr_name);
  shape_attr->value = malloc(strlen(cont) + 1);
  strcpy(shape_attr->value, cont);
  return shape_attr;
}

// Create a rectangle from the given node and add the rectangle to the given
// list.
static void createRectangle(List* parent_list, xmlNode* shape_node) {
  Rectangle* rect = malloc(sizeof(Rectangle));
  float value;
  char* unit;
  rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  rect->x = 0;
  rect->y = 0;
  rect->width = 0;
  rect->height = 0;
  (rect->units)[0] = '\0';
  for (xmlAttr* attr = shape_node->properties; attr != NULL; attr = attr->next) {
      xmlNode *child = attr->children;
      char *attr_name = (char *)attr->name;
      char *cont = (char *)(child->content);
      if (!strcmp(attr_name, "x")) {
        // Returns float from start of string, points unit at the first non numberical char
        value = strtof(cont, &unit);
        if ((rect->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(rect->units, unit);
        }
        rect->x = value;
      } else if (!strcmp(attr_name, "y")) {
        value = strtof(cont, &unit);
        if ((rect->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(rect->units, unit);
        }
        rect->y = value;
      } else if (!strcmp(attr_name, "width")) {
        value = strtof(cont, &unit);
        if ((rect->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(rect->units, unit);
        }
        rect->width = value;
      } else if (!strcmp(attr_name, "height")) {
        value = strtof(cont, &unit);
        if ((rect->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(rect->units, unit);
        }
        rect->height = value;
      } else {
        insertBack(rect->otherAttributes, createOtherAttribute(attr_name, cont));
      }
  }
  insertBack(parent_list, rect);
}

// Create a circle from the given node and add it to the given list.
static void createCircle(List* parent_list, xmlNode* shape_node) {
  Circle* circ = malloc(sizeof(Circle));
  char* unit;
  float value;
  circ->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  circ->cx = 0;
  circ->cy = 0;
  circ->r = 0;
  (circ->units)[0] = '\0';
  for (xmlAttr* attr = shape_node->properties; attr != NULL; attr = attr->next) {
      xmlNode *child = attr->children;
      char *attr_name = (char *)attr->name;
      char *cont = (char *)(child->content);
      if (!strcmp(attr_name, "cx")) {
        value = strtof(cont, &unit);
        if (((circ)->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(circ->units, unit);
        }
        circ->cx = value;
        stringTruncate(unit, 50);
        strcpy(circ->units, unit);
      } else if (!strcmp(attr_name, "cy")) {
        value = strtof(cont, &unit);
        if (((circ)->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(circ->units, unit);
        }
        circ->cy = value;
      } else if (!strcmp(attr_name, "r")) {
        value = strtof(cont, &unit);
        if (((circ)->units)[0] == '\0') {
          stringTruncate(unit, 50);
          strcpy(circ->units, unit);
        }
        circ->r = value;
      } else {
        insertBack(circ->otherAttributes, createOtherAttribute(attr_name, cont));
      }
  }
  insertBack(parent_list, circ);
}

// Create a path from the given node and store it in the given list.
static void createPath(List* parent_list, xmlNode* shape_node) {
  Path* the_path = malloc(sizeof(Path));
  the_path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  the_path->data = malloc(1);
  (the_path->data)[0] = '\0';
  for (xmlAttr* attr = shape_node->properties; attr != NULL; attr = attr->next) {
      xmlNode *value = attr->children;
      char *attr_name = (char *)attr->name;
      char *cont = (char *)(value->content);
      if (!strcmp(attr_name, "d")) {
        free(the_path->data);
        the_path->data = malloc(strlen(cont) + 1);
        strcpy(the_path->data, cont);
      } else {
        insertBack(the_path->otherAttributes, createOtherAttribute(attr_name, cont));
      }
  }
  insertBack(parent_list, the_path);
}

// Create a group from the given node and add it to the given list.
// This function is declared up here because fillGroup and createGroup need to
// be able to call each other.
static void createGroup(List* parent_list, xmlNode* group_node);

// Fill the given parent_group's lists with the types of node within it.
// Creates objects for each of the nodes belonging to the group.
// Traversal node contains the node of the child of the parent_group's node.
static void fillGroup(xmlNode* traversal_node, Group* parent_group) {
  for (; traversal_node != NULL; traversal_node = traversal_node->next) {
    if (!strcmp((char *) traversal_node->name, "rect")) {
      createRectangle(parent_group->rectangles, traversal_node);
    } else if (!strcmp((char *) traversal_node->name, "circle")) {
      createCircle(parent_group->circles, traversal_node);
    } else if (!strcmp((char *) traversal_node->name, "path")) {
      createPath(parent_group->paths, traversal_node);
    } else if (!strcmp((char *) traversal_node->name, "g")) {
      createGroup(parent_group->groups, traversal_node);
    }
  }
}

// Creates a group from the given group_node and stores it in the given list.
static void createGroup(List* parent_list, xmlNode* group_node) {
  Group* the_group = malloc(sizeof(Group));
  the_group->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
  the_group->circles = initializeList(circleToString, deleteCircle, compareCircles);
  the_group->paths = initializeList(pathToString, deletePath, comparePaths);
  the_group->groups = initializeList(groupToString, deleteGroup, compareGroups);
  the_group->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  for (xmlAttr* attr = group_node->properties; attr != NULL; attr = attr->next) {
      xmlNode *value = attr->children;
      char *attr_name = (char *)attr->name;
      char *cont = (char *)(value->content);
      insertBack(the_group->otherAttributes, createOtherAttribute(attr_name, cont));
  }
  fillGroup(group_node->children, the_group);
  insertBack(parent_list, the_group);
}

// Assign the proper values for the given SVGimage using the given svg_node.
static void addSVGInfo(SVGimage* svg, xmlNode* svg_node) {
  for (; svg_node != NULL; svg_node = svg_node->next) {
    if (!strcmp((char *) svg_node->name, "title")) {
      stringTruncate((char *) svg_node->children->content, 256);
      strcpy(svg->title, (char *) svg_node->children->content);
    } else if (!strcmp((char *) svg_node->name, "desc")) {
      stringTruncate((char *) svg_node->children->content, 256);
      strcpy(svg->description, (char *) svg_node->children->content);
    } else if (!strcmp((char *) svg_node->name, "rect")) {
      createRectangle(svg->rectangles, svg_node);
    } else if (!strcmp((char *) svg_node->name, "circle")) {
      createCircle(svg->circles, svg_node);
    } else if (!strcmp((char *) svg_node->name, "path")) {
      createPath(svg->paths, svg_node);
    } else if (!strcmp((char *) svg_node->name, "g")) {
      createGroup(svg->groups, svg_node);
    }
  }
}

// Create an SVGimage object using the svg file given by fileName.
SVGimage* createSVGimage(char* fileName) {
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  SVGimage *svg = NULL;
  doc = xmlReadFile(fileName, NULL, 0);
  if (doc == NULL) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  root_element = xmlDocGetRootElement(doc);
  if ((root_element->ns == NULL) || (root_element->ns->href == NULL)) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  svg = malloc(sizeof(SVGimage));
  svg->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
  svg->circles = initializeList(circleToString, deleteCircle, compareCircles);
  svg->paths = initializeList(pathToString, deletePath, comparePaths);
  svg->groups = initializeList(groupToString, deleteGroup, compareGroups);
  svg->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  stringTruncate((char *) root_element->ns->href, 256);
  strcpy(svg->namespace, (char *) root_element->ns->href);
  (svg->description)[0] = '\0';
  (svg->title)[0] = '\0';
  for (xmlAttr* attr = root_element->properties; attr != NULL; attr = attr->next) {
      xmlNode *value = attr->children;
      char *attr_name = (char *)attr->name;
      char *cont = (char *)(value->content);
      insertBack(svg->otherAttributes,createOtherAttribute(attr_name, cont));
  }
  addSVGInfo(svg, root_element->children);
  xmlFreeDoc(doc);
  xmlCleanupParser();
  return svg;
}

// Conver the entire SVGimage img into a readable string.
char* SVGimageToString(SVGimage* img) {
  char* final_string = NULL;
  if (img == NULL) {
    return NULL;
  }
  char* attributes_string = toString(img->otherAttributes);
  char* rect_string = toString(img->rectangles);
  char* circ_string = toString(img->circles);
  char* path_string = toString(img->paths);
  char* group_string = toString(img->groups);
  final_string = malloc(117 + strlen(img->title) + strlen(img->namespace) + strlen(img->description) + strlen(attributes_string) + strlen(rect_string) + strlen(circ_string) + strlen(path_string) + strlen(group_string));
  strcpy(final_string, "SVG Image Title: ");
  strcat(final_string, img->title);
  strcat(final_string, "\nNamespace: ");
  strcat(final_string, img->namespace);
  strcat(final_string, "\nDescription: ");
  strcat(final_string, img->description);
  strcat(final_string, "\nOther Attributes:\n");
  strcat(final_string, attributes_string);
  strcat(final_string, "\nRectangles:\n");
  strcat(final_string, rect_string);
  strcat(final_string, "Circles:\n");
  strcat(final_string, circ_string);
  strcat(final_string, "Paths:\n");
  strcat(final_string, path_string);
  strcat(final_string, "Groups:\n");
  strcat(final_string, group_string);
  strcat(final_string, "End of SVG Image\n");
  /* Use  this to find length
  printf("Actual malloc size: %d", (int) (strlen(final_string) + 1
  - (strlen(img->title) + strlen(img->namespace) + strlen(img->description)
  + strlen(attributes_string) + strlen(rect_string) + strlen(circ_string)
  + strlen(path_string) + strlen(group_string)))); */
  free(attributes_string);
  free(rect_string);
  free(circ_string);
  free(path_string);
  free(group_string);
  return final_string;
}

// Frees all malloced data in img.
void deleteSVGimage(SVGimage* img) {
  if (img == NULL) {
    return;
  }
  freeList(img->rectangles);
  freeList(img->circles);
  freeList(img->paths);
  freeList(img->groups);
  freeList(img->otherAttributes);
  free(img);
}

// Looks for groups in the given current_groups list and adds them to the
// all_groups list. Then runs this function with the found group's groups.
static void recursiveFindGroups(List* all_groups, List* current_groups) {
  Group* grp;
  ListIterator iter = createIterator(current_groups);
  while ((grp = nextElement(&iter)) != NULL){
    insertBack(all_groups, grp);
    recursiveFindGroups(all_groups, grp->groups);
  }
}

// Returns a list of all groups in img.
List* getGroups(SVGimage* img) {
  if (img == NULL) {
    return NULL;
  }
  List* all_groups = initializeList(groupToString, fakeDelete, compareGroups);
  recursiveFindGroups(all_groups, img->groups);
  return all_groups;
}

// Returns a list of all rectangles in img.
List* getRects(SVGimage* img) {
  Group* grp;
  Rectangle* rect;
  if (img == NULL) {
    return NULL;
  }
  List* all_rects = initializeList(rectangleToString, fakeDelete, compareRectangles);
  List* all_groups = getGroups(img);
  ListIterator rect_iter = createIterator(img->rectangles);
  while ((rect = nextElement(&rect_iter)) != NULL) {
    insertBack(all_rects, rect);
  }
  ListIterator grp_iter = createIterator(all_groups);
  while ((grp = nextElement(&grp_iter)) != NULL){
    rect_iter = createIterator(grp->rectangles);
    while ((rect = nextElement(&rect_iter)) != NULL) {
      insertBack(all_rects, rect);
    }
  }
  freeList(all_groups);
  return all_rects;
}

// Returns a list of all circles in img.
List* getCircles(SVGimage* img) {
  Group* grp;
  Circle* circ;
  if (img == NULL) {
    return NULL;
  }
  List* all_circs = initializeList(circleToString, fakeDelete, compareCircles);
  List* all_groups = getGroups(img);
  ListIterator circ_iter = createIterator(img->circles);
  while ((circ = nextElement(&circ_iter)) != NULL) {
    insertBack(all_circs, circ);
  }
  ListIterator grp_iter = createIterator(all_groups);
  while ((grp = nextElement(&grp_iter)) != NULL){
    circ_iter = createIterator(grp->circles);
    while ((circ = nextElement(&circ_iter)) != NULL) {
      insertBack(all_circs, circ);
    }
  }
  freeList(all_groups);
  return all_circs;
}

// Returns a list of all paths in img.
List* getPaths(SVGimage* img) {
  Group* grp;
  Path* the_path;
  if (img == NULL) {
    return NULL;
  }
  List* all_paths = initializeList(pathToString, fakeDelete, comparePaths);
  List* all_groups = getGroups(img);
  ListIterator the_path_iter = createIterator(img->paths);
  while ((the_path = nextElement(&the_path_iter)) != NULL) {
    insertBack(all_paths, the_path);
  }
  ListIterator grp_iter = createIterator(all_groups);
  while ((grp = nextElement(&grp_iter)) != NULL){
    the_path_iter = createIterator(grp->paths);
    while ((the_path = nextElement(&the_path_iter)) != NULL) {
      insertBack(all_paths, the_path);
    }
  }
  freeList(all_groups);
  return all_paths;
}

// Returns the number of all rectangles with the specified area.
int numRectsWithArea(SVGimage* img, float area) {
  int count = 0;
  if (img == NULL) {
    return 0;
  }
  List* all_rects = getRects(img);
  ListIterator iter = createIterator(all_rects);
  Rectangle* rect = nextElement(&iter);
  while (rect != NULL) {
    if (ceil(area) == (ceil(rect->width * rect->height))) {
      count++;
    }
    rect = nextElement(&iter);
  }
  freeList(all_rects);
  return count;
}

// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area) {
  int count = 0;
  if (img == NULL) {
    return 0;
  }
  List* all_circs = getCircles(img);
  ListIterator iter = createIterator(all_circs);
  Circle* circ = nextElement(&iter);
  while (circ != NULL) {
    if (ceil(area) == (ceil(circ->r * circ->r * PI))) {
      count++;
    }
    circ = nextElement(&iter);
  }
  freeList(all_circs);
  return count;
}

// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data) {
  int count = 0;
  if ((img == NULL) || (data == NULL)) {
    return 0;
  }
  List* all_paths = getPaths(img);
  ListIterator iter = createIterator(all_paths);
  Path* the_path = nextElement(&iter);
  while (the_path != NULL) {
    if (!strcmp(data, the_path->data)) {
      count++;
    }
    the_path = nextElement(&iter);
  }
  freeList(all_paths);
  return count;
}

// Returns the number of all groups with the specified length.
int numGroupsWithLen(SVGimage* img, int len) {
  int count = 0;
  if ((img == NULL) || (len < 0)) {
    return 0;
  }
  List* all_groups = getGroups(img);
  ListIterator iter = createIterator(all_groups);
  Group* the_group = nextElement(&iter);
  while (the_group != NULL) {
    if (len == (getLength(the_group->rectangles) + getLength(the_group->circles) + getLength(the_group->paths) + getLength(the_group->groups))) {
      count++;
    }
    the_group = nextElement(&iter);
  }
  freeList(all_groups);
  return count;
}

// Returns the amount of attributes in the entire SVGimage.
int numAttr(SVGimage* img) {
  if (img == NULL) {
    return 0;
  }
  int count = 0;

  List* all_rects = getRects(img);
  ListIterator iter = createIterator(all_rects);
  Rectangle* rect = nextElement(&iter);
  while (rect != NULL) {
    count = count + getLength(rect->otherAttributes);
    rect = nextElement(&iter);
  }
  freeList(all_rects);

  List* all_circs = getCircles(img);
  iter = createIterator(all_circs);
  Circle* circ = nextElement(&iter);
  while (circ != NULL) {
    count = count + getLength(circ->otherAttributes);
    circ = nextElement(&iter);
  }
  freeList(all_circs);

  List* all_paths = getPaths(img);
  iter = createIterator(all_paths);
  Path* the_path = nextElement(&iter);
  while (the_path != NULL) {
    count = count + getLength(the_path->otherAttributes);
    the_path = nextElement(&iter);
  }
  freeList(all_paths);

  List* all_groups = getGroups(img);
  iter = createIterator(all_groups);
  Group* the_group = nextElement(&iter);
  while (the_group != NULL) {
    count = count + getLength(the_group->otherAttributes);
    the_group = nextElement(&iter);
  }
  freeList(all_groups);
  count = count + getLength(img->otherAttributes);
  return count;
}

// Adds the list of attributes to the given node
bool addXmlAttributes(xmlNode* parent, List* attributes) {
  Attribute *attr = NULL;
  if (attributes == NULL) {
    return false;
  }
  ListIterator attr_iter = createIterator(attributes);
  while ((attr = nextElement(&attr_iter)) != NULL) {
    if ((attr->name == NULL) || (attr->value == NULL)) {
      return false;
    }
    xmlNewProp(parent, BAD_CAST attr->name, BAD_CAST attr->value);
  }
  return true;
}

// Adds the list of rectangles as children to the given node
bool addXmlRects(xmlNode* parent, List* rects) {
  char buffer[100];
  xmlNode *current = NULL;
  Rectangle *rect = NULL;
  if (rects == NULL) {
    return false;
  }
  ListIterator rect_iter = createIterator(rects);
  while ((rect = nextElement(&rect_iter)) != NULL) {
    if ((rect->width < 0) || (rect->height < 0)) {
      return false;
    }
    current = xmlNewChild(parent, NULL, BAD_CAST "rect", NULL);
    sprintf(buffer, "%f%s", rect->x, rect->units);
    xmlNewProp(current, BAD_CAST "x", BAD_CAST buffer);
    sprintf(buffer, "%f%s", rect->y, rect->units);
    xmlNewProp(current, BAD_CAST "y", BAD_CAST buffer);
    sprintf(buffer, "%f%s", rect->width, rect->units);
    xmlNewProp(current, BAD_CAST "width", BAD_CAST buffer);
    sprintf(buffer, "%f%s", rect->height, rect->units);
    xmlNewProp(current, BAD_CAST "height", BAD_CAST buffer);
    if (addXmlAttributes(current, rect->otherAttributes) == false) {
      return false;
    }
  }
  return true;
}

// Adds the list of circles as children to the given node
bool addXmlCircs(xmlNode* parent, List* circles) {
  char buffer[100];
  xmlNode *current = NULL;
  Circle *circ = NULL;
  if (circles == NULL) {
    return false;
  }
  ListIterator circ_iter = createIterator(circles);
  while ((circ = nextElement(&circ_iter)) != NULL) {
    if (circ->r < 0) {
      return false;
    }
    current = xmlNewChild(parent, NULL, BAD_CAST "circle", NULL);
    sprintf(buffer, "%f%s", circ->cx, circ->units);
    xmlNewProp(current, BAD_CAST "cx", BAD_CAST buffer);
    sprintf(buffer, "%f%s", circ->cy, circ->units);
    xmlNewProp(current, BAD_CAST "cy", BAD_CAST buffer);
    sprintf(buffer, "%f%s", circ->r, circ->units);
    xmlNewProp(current, BAD_CAST "r", BAD_CAST buffer);
    if (addXmlAttributes(current, circ->otherAttributes) == false) {
      return false;
    }
  }
  return true;
}

// Adds the list of paths as children to the given node
bool addXmlPaths(xmlNode* parent, List* paths) {
  xmlNode *current = NULL;
  Path *path = NULL;
  if (paths == NULL) {
    return false;
  }
  ListIterator path_iter = createIterator(paths);
  while ((path = nextElement(&path_iter)) != NULL) {
    if (path->data == NULL) {
      return false;
    }
    current = xmlNewChild(parent, NULL, BAD_CAST "path", NULL);
    xmlNewProp(current, BAD_CAST "d", BAD_CAST path->data);
    if (addXmlAttributes(current, path->otherAttributes) == false) {
      return false;
    }
  }
  return true;
}

// Adds the list of groups as children to the given node
bool addXmlGroups(xmlNode* parent, List* groups) {
  xmlNode *current = NULL;
  Group *group = NULL;
  if (groups == NULL) {
    return false;
  }
  ListIterator group_iter = createIterator(groups);
  while ((group = nextElement(&group_iter)) != NULL) {
    current = xmlNewChild(parent, NULL, BAD_CAST "g", NULL);
    if (addXmlAttributes(current, group->otherAttributes) == false) {
      return false;
    }
    if (addXmlRects(current, group->rectangles) == false) {
      return false;
    }
    if (addXmlCircs(current, group->circles) == false) {
      return false;
    }
    if (addXmlPaths(current, group->paths) == false) {
      return false;
    }
    if (addXmlGroups(current, group->groups) == false) {
      return false;
    }
  }
  return true;
}

// Creates an xmlDoc from the given SVGimage
xmlDoc* imageToDoc(SVGimage* img) {
  xmlNsPtr ns = NULL;
  xmlDoc *doc = NULL;
  xmlNode *root_node = NULL;
  if (img == NULL) {
    return NULL;
  }
  doc = xmlNewDoc(BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "svg");
  if ((img->namespace == NULL) || (img->namespace[0] == '\0')) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  ns = xmlNewNs(root_node, BAD_CAST img->namespace, NULL);
  xmlSetNs(root_node, ns);
  xmlDocSetRootElement(doc, root_node);
  if (addXmlAttributes(root_node, img->otherAttributes) == false) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  if ((img->title)[0] != '\0') {
    xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST img->title);
  }
  if ((img->description)[0] != '\0') {
    xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST img->description);
  }
  if (addXmlRects(root_node, img->rectangles) == false) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  if (addXmlCircs(root_node, img->circles) == false) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  if (addXmlPaths(root_node, img->paths) == false) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  if (addXmlGroups(root_node, img->groups) == false) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  xmlCleanupParser();
  return doc;
}

// Checks if the given xmlDoc is valid using the given schemafile
bool validateTree(xmlDoc* doc, char* schemaFile) {
  int ret;
  xmlSchemaParserCtxtPtr parserCtxt;
  xmlSchemaValidCtxtPtr validCtxt;
  xmlSchemaPtr schema = NULL;
  if ((doc == NULL) || (schemaFile == NULL) || (schemaFile[0] == '\0')) {
    return false;
  }
  parserCtxt = xmlSchemaNewParserCtxt(schemaFile);
  xmlSchemaSetParserErrors(parserCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
  schema = xmlSchemaParse(parserCtxt);
  xmlSchemaFreeParserCtxt(parserCtxt);
  if (schema == NULL) {
    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    return false;
  }
  validCtxt = xmlSchemaNewValidCtxt(schema);
  xmlSchemaSetValidErrors(validCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
  ret = xmlSchemaValidateDoc(validCtxt, doc);
  xmlSchemaFreeValidCtxt(validCtxt);
  if(schema != NULL) {
    xmlSchemaFree(schema);
  }
  xmlSchemaCleanupTypes();
  xmlCleanupParser();
  xmlMemoryDump();
  if (ret == 0)
  {
    return true;
  }
  return false;
}

// Checks if the given SVGimage is valid using the schemafile
bool validateSVGimage(SVGimage* image, char* schemaFile) {
  xmlDoc* doc = NULL;
  bool ret = 0;
  if ((image == NULL) || (schemaFile == NULL) || (schemaFile[0] == '\0')) {
    return false;
  }
  doc = imageToDoc(image);
  if (doc == NULL) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return false;
  }
  ret = validateTree(doc, schemaFile);
  xmlFreeDoc(doc);
  xmlCleanupParser();
  return ret;
}

// Creates a valid SVGimage from the given filename, returns NULL if it is not a valid SVG
SVGimage* createValidSVGimage(char* fileName, char* schemaFile) {
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  SVGimage *svg = NULL;
  if ((fileName == NULL) || (fileName[0] == '\0') || (schemaFile == NULL) || (schemaFile[0] == '\0')) {
    return NULL;
  }
  doc = xmlReadFile(fileName, NULL, 0);
  if (validateTree(doc, schemaFile) == false) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  root_element = xmlDocGetRootElement(doc);
  if ((root_element->ns == NULL) || (root_element->ns->href == NULL)) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return NULL;
  }
  svg = malloc(sizeof(SVGimage));
  svg->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
  svg->circles = initializeList(circleToString, deleteCircle, compareCircles);
  svg->paths = initializeList(pathToString, deletePath, comparePaths);
  svg->groups = initializeList(groupToString, deleteGroup, compareGroups);
  svg->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  stringTruncate((char *) root_element->ns->href, 256);
  strcpy(svg->namespace, (char *) root_element->ns->href);
  (svg->description)[0] = '\0';
  (svg->title)[0] = '\0';
  for (xmlAttr* attr = root_element->properties; attr != NULL; attr = attr->next) {
      xmlNode *value = attr->children;
      char *attr_name = (char *)attr->name;
      char *cont = (char *)(value->content);
      insertBack(svg->otherAttributes,createOtherAttribute(attr_name, cont));
  }
  addSVGInfo(svg, root_element->children);
  xmlFreeDoc(doc);
  xmlCleanupParser();
  if (validateSVGimage(svg, schemaFile) == true) {
    return svg;
  }
  deleteSVGimage(svg);
  return NULL;
}

// Creates an SVG file using the given SVGimage and filename
bool writeSVGimage(SVGimage* image, char* fileName) {
  if ((image == NULL) || (fileName == NULL) || (fileName[0] == '\0')) {
    return false;
  }
  xmlDoc* doc = imageToDoc(image);
  if (doc == NULL) {
    xmlCleanupParser();
    return false;
  }
  if (xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1) == -1) {
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return false;
  }
  xmlFreeDoc(doc);
  xmlCleanupParser();
  return true;
}

// Adds or replaces an attribute within attributes depending on if the newAttribute has the same name as an attribute in the list
void addOrReplaceAttr(List* attributes, Attribute* newAttribute) {
  Attribute* attr;
  bool done = false;
  ListIterator attr_iter = createIterator(attributes);
  while (((attr = nextElement(&attr_iter)) != NULL) && (!done)) {
    if (strcmp(attr->name, newAttribute->name) == 0) {
      free(attr->value);
      attr->value = malloc(strlen(newAttribute->value) + 1);
      strcpy(attr->value, newAttribute->value);
      deleteAttribute(newAttribute);
      done = true;
    }
  }
  if (done == false) {
    insertBack(attributes, newAttribute);
  }
}

// Adds or replaces an attribute in the given element using the image, element type and element index
void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {
  if ((newAttribute == NULL) || (image == NULL) || (newAttribute->name == NULL) || (newAttribute->value == NULL)) {
    return;
  }
  float value = 0;
  if (elemType == 0) {
    addOrReplaceAttr(image->otherAttributes, newAttribute);
    return;
  }
  int current = 0;
  ListIterator elem_iter;
  if (elemIndex < 0) {
    return;
  } else if (elemType == 1) {
    Circle *circ;
    elem_iter = createIterator(image->circles);
    circ = nextElement(&elem_iter);
    while ((circ != NULL) && (current != elemIndex)) {
      current++;
      circ = nextElement(&elem_iter);
    }
    if (circ == NULL) {
      return;
    }
    if (strcmp(newAttribute->name, "cx") == 0) {
      value = strtof(newAttribute->value, NULL);
      circ->cx = value;
      deleteAttribute(newAttribute);
    } else if (strcmp(newAttribute->name, "cy") == 0) {
      value = strtof(newAttribute->value, NULL);
      circ->cy = value;
      deleteAttribute(newAttribute);
    } else if (strcmp(newAttribute->name, "r") == 0) {
      value = strtof(newAttribute->value, NULL);
      circ->r = value;
      deleteAttribute(newAttribute);
    } else {
      addOrReplaceAttr(circ->otherAttributes, newAttribute);
    }
  } else if (elemType == 2) {
    Rectangle *rect;
    elem_iter = createIterator(image->rectangles);
    rect = nextElement(&elem_iter);
    while ((rect != NULL) && (current != elemIndex)) {
      current++;
      rect = nextElement(&elem_iter);
    }
    if (rect == NULL) {
      return;
    }
    if (strcmp(newAttribute->name, "x") == 0) {
      // Not sure how units work with This
      // Separate units and values then check if the units are the same
      // If the units given is unspecified then ??????, if the units of the rectangle are unspecified but the given value's units are specified then ??????
      value = strtof(newAttribute->value, NULL);
      rect->x = value;
      deleteAttribute(newAttribute);
    } else if (strcmp(newAttribute->name, "y") == 0) {
      value = strtof(newAttribute->value, NULL);
      rect->y = value;
      deleteAttribute(newAttribute);
    } else if (strcmp(newAttribute->name, "width") == 0) {
      value = strtof(newAttribute->value, NULL);
      rect->width = value;
      deleteAttribute(newAttribute);
    } else if (strcmp(newAttribute->name, "height") == 0) {
      value = strtof(newAttribute->value, NULL);
      rect->height = value;
      deleteAttribute(newAttribute);
    } else {
      addOrReplaceAttr(rect->otherAttributes, newAttribute);
    }
  } else if (elemType == 3) {
    Path *path;
    elem_iter = createIterator(image->paths);
    path = nextElement(&elem_iter);
    while ((path != NULL) && (current != elemIndex)) {
      current++;
      path = nextElement(&elem_iter);
    }
    if (path == NULL) {
      return;
    }
    if (strcmp(newAttribute->name, "d") == 0) {
      free(path->data);
      path->data = malloc(strlen(newAttribute->value) + 1);
      strcpy(path->data, newAttribute->value);
      deleteAttribute(newAttribute);
    } else {
      addOrReplaceAttr(path->otherAttributes, newAttribute);
    }
  } else if (elemType == 4) {
    Group *group;
    elem_iter = createIterator(image->groups);
    group = nextElement(&elem_iter);
    while ((group != NULL) && (current != elemIndex)) {
      current++;
      group = nextElement(&elem_iter);
    }
    if (group == NULL) {
      return;
    }
    addOrReplaceAttr(group->otherAttributes, newAttribute);
  }
}

// Adds the given element to the SVGimage
void addComponent(SVGimage* image, elementType type, void* newElement) {
  if ((image == NULL) || (newElement == NULL)) {
    return;
  }
  if (type == 1) {
    insertBack(image->circles, (Circle *) newElement);
  } else if (type == 2) {
    insertBack(image->rectangles, (Rectangle *) newElement);
  } else if (type == 3) {
    insertBack(image->paths, (Path *) newElement);
  }
}

// Converts attribute to JSON string
char* attrToJSON(const Attribute *a) {
  char* ret;
  if (a == NULL) {
    ret = malloc(3);
    strcpy(ret, "{}");
    return "{}";
  }
  ret = malloc(23 + strlen(a->name) + strlen(a->value));
  sprintf(ret, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
  return ret;
}

// Converts circle to JSON string
char* circleToJSON(const Circle *c) {
  char* ret;
  char buffer[200];
  if (c == NULL) {
    ret = malloc(3);
    strcpy(ret, "{}");
    return ret;
  }
  sprintf(buffer, "%.2f%.2f%.2f%d", c->cx, c->cy, c->r, getLength(c->otherAttributes));
  ret = malloc(strlen(buffer) + strlen(c->units) + 41);
  sprintf(ret, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, getLength(c->otherAttributes), c->units);
  return ret;
}

// Converts rectangle to JSON string
char* rectToJSON(const Rectangle *r) {
  char* ret;
  char buffer[200];
  if (r == NULL) {
    ret = malloc(3);
    strcpy(ret, "{}");
    return ret;
  }
  sprintf(buffer, "%.2f%.2f%.2f%.2f%d", r->x, r->y, r->width, r->height, getLength(r->otherAttributes));
  ret = malloc(strlen(buffer) + strlen(r->units) + 44);
  sprintf(ret, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units);
  return ret;
}

// Converts path to JSON string
char* pathToJSON(const Path *p) {
  char* ret;
  char buffer1[65];
  char buffer2[200];
  if (p == NULL) {
    ret = malloc(3);
    strcpy(ret, "{}");
    return ret;
  }
  if (strlen(p->data) <= 64) {
    strcpy(buffer1, p->data);
  } else {
    strncpy(buffer1, p->data, 64);
    buffer1[64] = '\0';
  }
  sprintf(buffer2, "%d", getLength(p->otherAttributes));
  ret = malloc(strlen(buffer1) + strlen(buffer2) + 20);
  sprintf(ret, "{\"d\":\"%s\",\"numAttr\":%s}", buffer1, buffer2);
  return ret;
}

// Converts group to JSON string
char* groupToJSON(const Group *g) {
  char* ret;
  char buffer[200];
  if (g == NULL) {
    ret = malloc(3);
    strcpy(ret, "{}");
    return ret;
  }
  sprintf(buffer, "%d%d", (getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths) + getLength(g->groups)), getLength(g->otherAttributes));
  ret = malloc(strlen(buffer) + 25);
  sprintf(ret, "{\"children\":%d,\"numAttr\":%d}", (getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths) + getLength(g->groups)), getLength(g->otherAttributes));
  return ret;
}

// Converts SVG to jSON string
char* SVGtoJSON(const SVGimage* imge) {
  char* ret;
  List* rects;
  List* circs;
  List* paths;
  List* groups;
  char buffer[400];
  if (imge == NULL) {
    ret = malloc(3);
    strcpy(ret, "{}");
    return ret;
  }
  rects = getRects((SVGimage *) imge);
  circs = getCircles((SVGimage *) imge);
  paths = getPaths((SVGimage *) imge);
  groups = getGroups((SVGimage *) imge);
  sprintf(buffer, "%d%d%d%d", getLength(rects), getLength(circs), getLength(paths), getLength(groups));
  ret = malloc(strlen(buffer) + 49);
  sprintf(ret, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", getLength(rects), getLength(circs), getLength(paths), getLength(groups));
  freeList(rects);
  freeList(circs);
  freeList(paths);
  freeList(groups);
  return ret;
}

char* svgJSONFromName(char *filename) {
  SVGimage* img = createValidSVGimage(filename, "parser/svg.xsd");
  char *json = SVGtoJSON(img);
  deleteSVGimage(img);
  return json;
}

char* getTitleAndDescFromName(char *filename) {
  char buffer[700];
  SVGimage* img = createValidSVGimage(filename, "parser/svg.xsd");
  int length = strlen(img->title);
  for (int i = 0; i < length; i++) {
    if (img->title[i] == '"') {
      img->title[i] = '\'';
    } else if (img->title[i] == '\n') {
      img->title[i] = ' ';
    }
  }
  length = strlen(img->description);
  for (int i = 0; i < length; i++) {
    if (img->description[i] == '"') {
      img->description[i] = '\'';
    } else if (img->description[i] == '\n') {
      img->description[i] = ' ';
    }
  }
  sprintf(buffer, "{\"title\":\"%s\",\"desc\":\"%s\"}", img->title, img->description);
  char *retStr = malloc(strlen(buffer) + 1);
  strcpy(retStr, buffer);
  deleteSVGimage(img);
  return retStr;
}

char* componentsJSON(char *filename) {
  SVGimage* img = createValidSVGimage(filename, "parser/svg.xsd");
  char *ret;
  int comps_size = 0;
  int comps = 4;
  char *rectString = rectListToJSON(img->rectangles);
  comps_size += strlen(rectString);
  char *circString = circListToJSON(img->circles);
  comps_size += strlen(circString);
  char *pathString = pathListToJSON(img->paths);
  comps_size += strlen(pathString);
  char *groupString = groupListToJSON(img->groups);
  comps_size += strlen(groupString);
  ret = malloc(2 + comps_size + comps - 1 + 1);
  ret[0] = '[';
  ret[1] = '\0';
  strcat(ret, rectString);
  strcat(ret, ",");
  free(rectString);
  strcat(ret, circString);
  strcat(ret, ",");
  free(circString);
  strcat(ret, pathString);
  strcat(ret, ",");
  free(pathString);
  strcat(ret, groupString);
  strcat(ret, ",");
  free(groupString);
  ret[2 + comps_size + comps - 1 + 1 - 2] = ']'; // second last index becomes ]
  ret[2 + comps_size + comps - 1 + 1 - 1] = '\0'; // last index becomes delimiter
  return ret;
}

// Converts list of attributes to JSON string
char* attrListToJSON(const List *list) {
  char* ret;
  char* jsonString;
  int attrs_size = 0;
  int attrs = 0;
  if ((list == NULL) || (getLength((List *) list) == 0)) {
    ret = malloc(3);
    strcpy(ret, "[]");
    return ret;
  }
  Attribute* attr;
  ListIterator attr_iter = createIterator((List *) list);
  while ((attr = nextElement(&attr_iter)) != NULL) {
    jsonString = attrToJSON(attr);
    attrs_size += strlen(jsonString);
    free(jsonString);
    attrs++;
  }
  /* 2 square brackets + length of all attr strings + a comma for each attr
  string except for the last + string delimiter */
  ret = malloc(2 + attrs_size + attrs - 1 + 1);
  ret[0] = '[';
  ret[1] = '\0';
  attr_iter = createIterator((List *) list);
  while ((attr = nextElement(&attr_iter)) != NULL) {
    jsonString = attrToJSON(attr);
    strcat(ret, jsonString);
    strcat(ret, ",");
    free(jsonString);
  }
  ret[2 + attrs_size + attrs - 1 + 1 - 2] = ']'; // second last index becomes ]
  ret[2 + attrs_size + attrs - 1 + 1 - 1] = '\0'; // last index becomes delimiter
  return ret;
}

// Converts list of circles to JSON string
char* circListToJSON(const List *list) {
  char* ret;
  char* jsonString;
  int circs_size = 0;
  int circs = 0;
  if ((list == NULL) || (getLength((List *) list) == 0)) {
    ret = malloc(3);
    strcpy(ret, "[]");
    return ret;
  }
  Circle* circ;
  ListIterator circ_iter = createIterator((List *) list);
  while ((circ = nextElement(&circ_iter)) != NULL) {
    jsonString = circleToJSON(circ);
    circs_size += strlen(jsonString);
    free(jsonString);
    circs++;
  }
  /* 2 square brackets + length of all circ strings + a comma for each circ
  string except for the last + string delimiter */
  ret = malloc(2 + circs_size + circs - 1 + 1);
  ret[0] = '[';
  ret[1] = '\0';
  circ_iter = createIterator((List *) list);
  while ((circ = nextElement(&circ_iter)) != NULL) {
    jsonString = circleToJSON(circ);
    strcat(ret, jsonString);
    strcat(ret, ",");
    free(jsonString);
  }
  ret[2 + circs_size + circs - 1 + 1 - 2] = ']'; // second last index becomes ]
  ret[2 + circs_size + circs - 1 + 1 - 1] = '\0'; // last index becomes delimiter
  return ret;
}

// Converts list of rectangles to JSON string
char* rectListToJSON(const List *list) {
  char* ret;
  char* jsonString;
  int rects_size = 0;
  int rects = 0;
  if ((list == NULL) || (getLength((List *) list) == 0)) {
    ret = malloc(3);
    strcpy(ret, "[]");
    return ret;
  }
  Rectangle* rect;
  ListIterator rect_iter = createIterator((List *) list);
  while ((rect = nextElement(&rect_iter)) != NULL) {
    jsonString = rectToJSON(rect);
    rects_size += strlen(jsonString);
    free(jsonString);
    rects++;
  }
  /* 2 square brackets + length of all rect strings + a comma for each rect
  string except for the last + string delimiter */
  ret = malloc(2 + rects_size + rects - 1 + 1);
  ret[0] = '[';
  ret[1] = '\0';
  rect_iter = createIterator((List *) list);
  while ((rect = nextElement(&rect_iter)) != NULL) {
    jsonString = rectToJSON(rect);
    strcat(ret, jsonString);
    strcat(ret, ",");
    free(jsonString);
  }
  ret[2 + rects_size + rects - 1 + 1 - 2] = ']'; // second last index becomes ]
  ret[2 + rects_size + rects - 1 + 1 - 1] = '\0'; // last index becomes delimiter
  return ret;
}

// Converts list of paths to JSON string
char* pathListToJSON(const List *list) {
  char* ret;
  char* jsonString;
  int paths_size = 0;
  int paths = 0;
  if ((list == NULL) || (getLength((List *) list) == 0)) {
    ret = malloc(3);
    strcpy(ret, "[]");
    return ret;
  }
  Path* path;
  ListIterator path_iter = createIterator((List *) list);
  while ((path = nextElement(&path_iter)) != NULL) {
    jsonString = pathToJSON(path);
    paths_size += strlen(jsonString);
    free(jsonString);
    paths++;
  }
  /* 2 square brackets + length of all path strings + a comma for each path
  string except for the last + string delimiter */
  ret = malloc(2 + paths_size + paths - 1 + 1);
  ret[0] = '[';
  ret[1] = '\0';
  path_iter = createIterator((List *) list);
  while ((path = nextElement(&path_iter)) != NULL) {
    jsonString = pathToJSON(path);
    strcat(ret, jsonString);
    strcat(ret, ",");
    free(jsonString);
  }
  ret[2 + paths_size + paths - 1 + 1 - 2] = ']'; // second last index becomes ]
  ret[2 + paths_size + paths - 1 + 1 - 1] = '\0'; // last index becomes delimiter
  return ret;
}

// Converts list of groups to JSON string
char* groupListToJSON(const List *list) {
  char* ret;
  char* jsonString;
  int groups_size = 0;
  int groups = 0;
  if ((list == NULL) || (getLength((List *) list) == 0)) {
    ret = malloc(3);
    strcpy(ret, "[]");
    return ret;
  }
  Group* group;
  ListIterator group_iter = createIterator((List *) list);
  while ((group = nextElement(&group_iter)) != NULL) {
    jsonString = groupToJSON(group);
    groups_size += strlen(jsonString);
    free(jsonString);
    groups++;
  }
  /* 2 square brackets + length of all group strings + a comma for each group
  string except for the last + string delimiter */
  ret = malloc(2 + groups_size + groups - 1 + 1);
  ret[0] = '[';
  ret[1] = '\0';
  group_iter = createIterator((List *) list);
  while ((group = nextElement(&group_iter)) != NULL) {
    jsonString = groupToJSON(group);
    strcat(ret, jsonString);
    strcat(ret, ",");
    free(jsonString);
  }
  ret[2 + groups_size + groups - 1 + 1 - 2] = ']'; // second last index becomes ]
  ret[2 + groups_size + groups - 1 + 1 - 1] = '\0'; // last index becomes delimiter
  return ret;
}

// Converts JSON string of SVG to an SVG
SVGimage* JSONtoSVG(const char* svgString) {
  if (svgString == NULL) {
    return NULL;
  }
  int i = 0;
  int start = -1;
  int end = -1;
  char* title;
  char* description;
  SVGimage* img;
  while ((start == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == ':') {
      start = i + 2;
    }
    i++;
  }
  if (start == -1) {
    return NULL;
  }
  while ((end == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == ',') {
      end = i - 2;
    }
    i++;
  }
  if (end == -1) {
    return NULL;
  }
  if ((end - start + 1) > 255) {
    end = 255 + start - 1;
  }
  title = malloc(end - start + 2);
  strncpy(title, (svgString + start), end - start + 1);
  title[end - start + 1] = '\0';
  start = -1;
  end = -1;
  while ((start == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == ':') {
      start = i + 2;
    }
    i++;
  }
  if (start == -1) {
    return NULL;
  }
  i++;
  while ((end == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == '"') {
      end = i - 1;
    }
    i++;
  }
  if (end == -1) {
    return NULL;
  }
  if ((end - start + 1) > 255) {
    end = 255 + start - 1;
  }
  description = malloc(end - start + 2);
  strncpy(description, (svgString + start), end - start + 1);
  description[end - start + 1] = '\0';
  img = malloc(sizeof(SVGimage));
  strcpy(img->description, description);
  strcpy(img->title, title);
  strcpy(img->namespace, "http://www.w3.org/2000/svg");
  img->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
  img->circles = initializeList(circleToString, deleteCircle, compareCircles);
  img->paths = initializeList(pathToString, deletePath, comparePaths);
  img->groups = initializeList(groupToString, deleteGroup, compareGroups);
  img->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  return img;
}

// Finds the first value starting at nextChar, returns it and updates nextChar
char* findValue(char* jsonString, int* nextChar) {
  char* value;
  int start = -1;
  int end = -1;
  int i = *nextChar;
  while ((start == -1) && (jsonString[i] != '\0')) {
    if (jsonString[i] == ':') {
      start = i + 1;
    }
    i++;
  }
  if (start == -1) {
    return NULL;
  }
  while ((end == -1) && (jsonString[i] != '\0')) {
    if (jsonString[i] == ',') {
      end = i - 1;
    }
    i++;
  }
  if (end == -1) {
    return NULL;
  }
  if ((end - start + 1) > 255) {
    end = 255 + start - 1;
  }
  value = malloc(end - start + 2);
  strncpy(value, (jsonString + start), end - start + 1);
  value[end - start + 1] = '\0';
  *nextChar = i;
  return value;
}

// Converts JSON string of rectangle to a Rectangle
Rectangle* JSONtoRect(const char* svgString) {
  Rectangle* rect = malloc(sizeof(Rectangle));
  int start = 0;
  int end;
  int i;
  float fValue;
  char* value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  rect->x = fValue;
  value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  rect->y = fValue;
  value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  rect->width = fValue;
  value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  rect->height = fValue;
  i = start;
  start = -1;
  end = -1;
  while ((start == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == ':') {
      start = i + 2;
    }
    i++;
  }
  if (start == -1) {
    return NULL;
  }
  i++;
  while ((end == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == '"') {
      end = i - 1;
    }
    i++;
  }
  if (end == -1) {
    return NULL;
  }
  if ((end - start + 1) > 50) {
    end = 50 + start - 1;
  }
  value = malloc(end - start + 2);
  strncpy(value, (svgString + start), end - start + 1);
  value[end - start + 1] = '\0';
  strcpy(rect->units, value);
  rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  return rect;
}

// Converts JSON string of circle to a Circle
Circle* JSONtoCircle(const char* svgString) {
  Circle* circ = malloc(sizeof(Circle));
  int start = 0;
  int end;
  int i;
  float fValue;
  char* value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  circ->cx = fValue;
  value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  circ->cy = fValue;
  value = findValue((char *) svgString, &start);
  if (value == NULL) {
    return NULL;
  }
  fValue = strtof(value, NULL);
  free(value);
  circ->r = fValue;
  i = start;
  start = -1;
  end = -1;
  while ((start == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == ':') {
      start = i + 2;
    }
    i++;
  }
  if (start == -1) {
    return NULL;
  }
  i++;
  while ((end == -1) && (svgString[i] != '\0')) {
    if (svgString[i] == '"') {
      end = i - 1;
    }
    i++;
  }
  if (end == -1) {
    return NULL;
  }
  if ((end - start + 1) > 50) {
    end = 50 + start - 1;
  }
  value = malloc(end - start + 2);
  strncpy(value, (svgString + start), end - start + 1);
  value[end - start + 1] = '\0';
  strcpy(circ->units, value);
  circ->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  return circ;
}

int checkValid(char *filename) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  if (svg == NULL) {
    return 0;
  } else {
    deleteSVGimage(svg);
    return 1;
  }
}

char *attributesSVG(char *filename) {
  char buffer[1024];
  char small_buffer[200];
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  sprintf(buffer, "{");
  ListIterator iter = createIterator((List *) svg->otherAttributes);
  Attribute *attr;
  while ((attr = nextElement(&iter)) != NULL) {
    sprintf(small_buffer, "\"%s\":\"%s\",", attr->name, attr->value);
    strcat(buffer, small_buffer);
  }
  int length = strlen(buffer);
  buffer[length - 1] = '}';
  buffer[length] = '\0';
  deleteSVGimage(svg);
  char *retstr = malloc(strlen(buffer) + 1);
  strcpy(retstr, buffer);
  return retstr;
}

char *attributesRectangle(char *filename, int rect_index) {
  char buffer[1024];
  char small_buffer[200];
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Rectangle* rect;
  ListIterator iter = createIterator((List *) svg->rectangles);
  while (rect_index > 0) {
    rect = nextElement(&iter);
    rect_index--;
  }
  sprintf(buffer, "{\"x\":%.2f,\"y\":%.2f,\"width\":%.2f,\"height\":%.2f", rect->x, rect->y, rect->width, rect->height);
  iter = createIterator((List *) rect->otherAttributes);
  Attribute *attr;
  while ((attr = nextElement(&iter)) != NULL) {
    sprintf(small_buffer, ",\"%s\":\"%s\"", attr->name, attr->value);
    strcat(buffer, small_buffer);
  }
  int length = strlen(buffer);
  buffer[length] = '}';
  buffer[length + 1] = '\0';
  deleteSVGimage(svg);
  char *retstr = malloc(strlen(buffer) + 1);
  strcpy(retstr, buffer);
  return retstr;
}

char *attributesCircle(char *filename, int circ_index) {
  char buffer[1024];
  char small_buffer[200];
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Circle* circ;
  ListIterator iter = createIterator((List *) svg->circles);
  while (circ_index > 0) {
    circ = nextElement(&iter);
    circ_index--;
  }
  sprintf(buffer, "{\"cx\":%.2f,\"cy\":%.2f,\"radius\":%.2f", circ->cx, circ->cy, circ->r);
  iter = createIterator((List *) circ->otherAttributes);
  Attribute *attr;
  while ((attr = nextElement(&iter)) != NULL) {
    sprintf(small_buffer, ",\"%s\":\"%s\"", attr->name, attr->value);
    strcat(buffer, small_buffer);
  }
  int length = strlen(buffer);
  buffer[length] = '}';
  buffer[length + 1] = '\0';
  deleteSVGimage(svg);
  char *retstr = malloc(strlen(buffer) + 1);
  strcpy(retstr, buffer);
  return retstr;
}

char *attributesPath(char *filename, int path_index) {
  char buffer[1024];
  char small_buffer[200];
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Path* path;
  ListIterator iter = createIterator((List *) svg->paths);
  while (path_index > 0) {
    path = nextElement(&iter);
    path_index--;
  }
  sprintf(buffer, "{\"data\":\"%s\"", path->data);
  iter = createIterator((List *) path->otherAttributes);
  Attribute *attr;
  while ((attr = nextElement(&iter)) != NULL) {
    sprintf(small_buffer, ",\"%s\":\"%s\"", attr->name, attr->value);
    strcat(buffer, small_buffer);
  }
  int length = strlen(buffer);
  buffer[length] = '}';
  buffer[length + 1] = '\0';
  deleteSVGimage(svg);
  char *retstr = malloc(strlen(buffer) + 1);
  strcpy(retstr, buffer);
  return retstr;
}

char *attributesGroup(char *filename, int group_index) {
  char buffer[1024];
  char small_buffer[200];
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Group* group;
  ListIterator iter = createIterator((List *) svg->groups);
  while (group_index > 0) {
    group = nextElement(&iter);
    group_index--;
  }
  sprintf(buffer, "{");
  iter = createIterator((List *) group->otherAttributes);
  Attribute *attr;
  while ((attr = nextElement(&iter)) != NULL) {
    sprintf(small_buffer, "\"%s\":\"%s\",", attr->name, attr->value);
    strcat(buffer, small_buffer);
  }
  int length = strlen(buffer);
  buffer[length - 1] = '}';
  buffer[length] = '\0';
  deleteSVGimage(svg);
  char *retstr = malloc(strlen(buffer) + 1);
  strcpy(retstr, buffer);
  return retstr;
}

int setAttributeSVG(char *filename, int attr_index, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  char *name;
  Attribute *attr2;
  ListIterator iter = createIterator((List *) svg->otherAttributes);
  int counter = -1;
  while (counter < attr_index) {
    attr2 = nextElement(&iter);
    counter++;
  }
  name = malloc(strlen(attr2->name) + 1);
  strcpy(name, attr2->name);

  attr = createOtherAttribute(name, value);
  setAttribute(svg, 0, 0, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

// setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute
int setAttributeRectangle(char *filename, int elem_index, int attr_index, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  char *name;
  // x, y, width, height, units,
  if (attr_index == 0) {
    name = malloc(2);
    name[0] = 'x';
    name[1] = '\0';
  } else if (attr_index == 1) {
    name = malloc(2);
    name[0] = 'y';
    name[1] = '\0';
  } else if (attr_index == 2) {
    name = malloc(6);
    strcpy(name, "width");
  } else if (attr_index == 3) {
    name = malloc(6);
    strcpy(name, "height");
  } else {
    int counter = elem_index;
    Rectangle* elem;
    ListIterator iter = createIterator((List *) svg->rectangles);
    while (counter >= 0) {
      elem = nextElement(&iter);
      counter--;
    }
    Attribute *attr;
    iter = createIterator((List *) elem->otherAttributes);
    counter = 3;
    while (counter < attr_index) {
      attr = nextElement(&iter);
      counter++;
    }
    name = malloc(strlen(attr->name) + 1);
    strcpy(name, attr->name);
  }
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 2, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int setAttributeCircle(char *filename, int elem_index, int attr_index, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  char *name;
  // cx, cy, r
  if (attr_index == 0) {
    name = malloc(3);
    name[0] = 'c';
    name[1] = 'x';
    name[2] = '\0';
  } else if (attr_index == 1) {
    name = malloc(3);
    name[0] = 'c';
    name[1] = 'y';
    name[2] = '\0';
  } else if (attr_index == 2) {
    name = malloc(2);
    strcpy(name, "r");
  } else {
    int counter = elem_index;
    Circle* elem;
    ListIterator iter = createIterator((List *) svg->circles);
    while (counter >= 0) {
      elem = nextElement(&iter);
      counter--;
    }
    Attribute *attr;
    iter = createIterator((List *) elem->otherAttributes);
    counter = 2;
    while (counter < attr_index) {
      attr = nextElement(&iter);
      counter++;
    }
    name = malloc(strlen(attr->name) + 1);
    strcpy(name, attr->name);
  }
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 1, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int setAttributePath(char *filename, int elem_index, int attr_index, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  char *name;
  // d
  if (attr_index == 0) {
    name = malloc(2);
    name[0] = 'd';
    name[1] = '\0';
  } else {
    int counter = elem_index;
    Path* elem;
    ListIterator iter = createIterator((List *) svg->paths);
    while (counter >= 0) {
      elem = nextElement(&iter);
      counter--;
    }
    Attribute *attr;
    iter = createIterator((List *) elem->otherAttributes);
    counter = 0;
    while (counter < attr_index) {
      attr = nextElement(&iter);
      counter++;
    }
    name = malloc(strlen(attr->name) + 1);
    strcpy(name, attr->name);
  }
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 3, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int setAttributeGroup(char *filename, int elem_index, int attr_index, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  char *name;
  int counter = elem_index;
  Group* elem;
  ListIterator iter = createIterator((List *) svg->groups);
  while (counter >= 0) {
    elem = nextElement(&iter);
    counter--;
  }
  Attribute *attr2;
  iter = createIterator((List *) elem->otherAttributes);
  counter = -1;
  while (counter < attr_index) {
    attr2 = nextElement(&iter);
    counter++;
  }
  name = malloc(strlen(attr2->name) + 1);
  strcpy(name, attr2->name);

  attr = createOtherAttribute(name, value);
  setAttribute(svg, 4, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int newAttributeSVG(char *filename, char *name, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 0, 0, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int newAttributeRectangle(char *filename, int elem_index, char *name, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 2, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int newAttributeCircle(char *filename, int elem_index, char* name, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 1, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int newAttributePath(char *filename, int elem_index, char* name, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 3, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int newAttributeGroup(char *filename, int elem_index, char *name, char *value) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  Attribute *attr;
  attr = createOtherAttribute(name, value);
  setAttribute(svg, 4, elem_index, attr);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int changeTitleAndDesc(char *filename, char *title, char *desc) {
  SVGimage *svg = createValidSVGimage(filename, "parser/svg.xsd");
  stringTruncate(title, 256);
  strcpy(svg->title, title);
  stringTruncate(desc, 256);
  strcpy(svg->description, desc);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    writeSVGimage(svg, filename);
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}

int createSVG(char *filename) {
  if (filename[0] == '\0') {
    return 0;
  }
  SVGimage *svg = malloc(sizeof(SVGimage));
  strcpy(svg->namespace, "http://www.w3.org/2000/svg");
  svg->title[0] = '\0';
  svg->description[0] = '\0';
  svg->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
  svg->circles = initializeList(circleToString, deleteCircle, compareCircles);
  svg->paths = initializeList(pathToString, deletePath, comparePaths);
  svg->groups = initializeList(groupToString, deleteGroup, compareGroups);
  svg->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  Attribute *attr = malloc(sizeof(Attribute));
  attr->name = malloc(8);
  strcpy(attr->name, "viewBox");
  attr->value = malloc(13);
  strcpy(attr->value, "0 0 1200 600");
  insertBack(svg->otherAttributes, attr);
  Rectangle* rect = malloc(sizeof(Rectangle));
  rect->x = 0;
  rect->y = 0;
  rect->width = 1;
  rect->height = 1;
  strcpy(rect->units, "cm");
  rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
  insertBack(svg->rectangles, rect);
  bool valid = validateSVGimage(svg, "parser/svg.xsd");
  if (valid == true) {
    if (writeSVGimage(svg, filename) == false) {
      deleteSVGimage(svg);
      return 0;
    }
    deleteSVGimage(svg);
    return 1;
  } else {
    deleteSVGimage(svg);
    return 0;
  }
}
