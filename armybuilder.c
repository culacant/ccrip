void get_label_content_units(int labelid,char *out)
{
  switch(labelid)
  {
    case UNITCOUNT_LABEL:
      sprintf(out,"%i",NUMUNITS);
      break;
    case TEAMCOUNT_LABEL:
      sprintf(out,"%i",NUMTEAMS);
      break;
  }
}
// armybuilder

#define NUMARMYBUILDER_GUI_LABELS 15
enum ARMYBUILDER_GUI_LABELS_SECTIONS
{
  SECTION_START         = 0,
  SECTION_UNITS         = 2,
  SECTION_UNIT          = 7,
  SECTION_FIGURE        = 15,
};
enum ARMYBUILDER_GUI_LABELS_ID
{
  UNITCOUNT_LABEL       = 0,
  TEAMCOUNT_LABEL       = 1,

  UNIT_LABEL            = 2,
  TEAM_LABEL            = 3,
  COMMANDLEVEL_LABEL    = 4,
  UFLAGS_LABEL          = 5,
  FIGURECNT_LABEL       = 6,

  FIGURE_LABEL          = 7,
  FP_LABEL              = 8,
  IV_LABEL              = 9,
  AT_LABEL              = 10,
  LV_LABEL              = 11,
  QUALITY_LABEL         = 12,
  CONFIDENCE_LABEL      = 13,
  FFLAGS_LABEL          = 14,
// 15 inbetween
  FIGURE_TABLE_OFFSET   = 30,
};

// armybuilder
Rectangle ARMYBUILDER_LABEL_RECTANGLES[] =
{
// TITLE
  {20 ,20 ,100  ,20},  //  UNITCOUNT_TITLE
  {20 ,50 ,100  ,20},  //  TEAMCOUNT_TITLE

  {350 ,80 ,100  ,20},  //  UNIT_TITLE
  {350 ,100,100  ,20},  //  TEAM_TITLE
  {350 ,120,100  ,20},  //  COMMANDLEVEL_TITLE
  {350 ,140,100  ,20},  //  UFLAGS_TITLE
  {350 ,160,100  ,20},  //  FIGURECNT_TITLE

  { 350 ,220,100  ,20},  //  FIGURE_TITLE
  { 450 ,220,100  ,20},  //  FP_TITLE
  { 550 ,220,100  ,20},  //  IV_TITLE
  { 650 ,220,100  ,20},  //  AT_TITLE
  { 750 ,220,100  ,20},  //  LV_TITLE
  { 850 ,220,100  ,20},  //  QUALITY_TITLE
  { 950 ,220,100  ,20},  //  CONFIDENCE_TITLE
  {1050 ,220,100  ,20},  //  FFLAGS_TITLE
// CONTENT
  {130 ,20 ,100  ,20},  //  UNITCOUNT_CONTENT
  {130 ,50 ,100  ,20},  //  TEAMCOUNT_CONTENT

  {430 ,80 ,100  ,20},  //  UNIT_CONTENT
  {430 ,100,100  ,20},  //  TEAM_CONTENT
  {430 ,120,100  ,20},  //  COMMANDLEVEL_CONTENT
  {430 ,140,100  ,20},  //  UFLAGS_CONTENT
  {430 ,160,100  ,20},  //  FIGURECNT_CONTENT

  { 350 ,240,100  ,20},  //  FIGURE_CONTENT
  { 450 ,240,100  ,20},  //  FP_CONTENT
  { 550 ,240,100  ,20},  //  IV_CONTENT
  { 650 ,240,100  ,20},  //  AT_CONTENT
  { 750 ,240,100  ,20},  //  LV_CONTENT
  { 850 ,240,100  ,20},  //  QUALITY_CONTENT
  { 950 ,240,100  ,20},  //  CONFIDENCE_CONTENT
  {1050 ,240,100  ,20},  //  FFLAGS_CONTENT

  {0    , 20,  0  , 0},  //  FIGURE_TABLE_OFFSET
};
char *ARMYBUILDER_LABEL_TITLE[] =
{
  "total unit count:",  //  UNITCOUNT_TITLE
  "unit count for team:",  //  TEAMCOUNT_TITLE

  "unit name:",  //  UNIT_TITLE
  "team #:",  //  TEAM_TITLE
  "commandlevel:",  //  COMMANDLEVEL_TITLE
  "unit flags:",  //  UFLAGS_TITLE
  "figure count:",  //  FIGURECNT_TITLE

  "figure name:",  //  FIGURE_TITLE
  "firepower die:",  //  FP_TITLE
  "impact die:",  //  IV_TITLE
  "armor type:",  //  AT_TITLE
  "leadership value:",  //  LV_TITLE
  "quality:",  //  QUALITY_TITLE
  "confidence:",  //  CONFIDENCE_TITLE
  "figure flags:",  //  FFLAGS_TITLE
};
void get_label_content_units(int labelid,char *out);
void get_label_content_unit(Unit unit,int labelid,char *out);
void get_label_content_figure(Figure figure,int labelid,char *out);

void draw_gui_elements(Unit *units);
void run_armybuilder(Unit *units);
Rectangle RectangleAdd(Rectangle rec1, Rectangle rec2);






void get_label_content_unit(Unit unit,int labelid,char *out)
{
  char in[256];
  switch(labelid)
  {
    case UNIT_LABEL:
      strcpy(out,"NAME");
      break;
    case TEAM_LABEL:
      sprintf(out,"%i",unit.team);
      break;
    case COMMANDLEVEL_LABEL:
      commandlevel_enum_to_string(in,unit.commandlevel);
      strcpy(out,in);
      break;
    case UFLAGS_LABEL:
      flags_to_string(in,unit.flags);
      strcpy(out,in);
      break;
    case FIGURECNT_LABEL:
      sprintf(out,"%i",unit.figurecnt);
      break;
  }
}
void get_label_content_figure(Figure figure,int labelid,char *out)
{
  /*
  char in[256];
  switch(labelid)
  {
    case FIGURE_LABEL:
      strcpy(out,"NAME");
      break;
    case FP_LABEL:
      sprintf(out,"D%i",figure.FP);
      break;
    case IV_LABEL:
      sprintf(out,"D%i",figure.IV);
      break;
    case AT_LABEL:
      AT_enum_to_string(in,figure.AT);
      strcpy(out,in);
      break;
    case LV_LABEL:
      lv_enum_to_string(in,figure.LV);
      strcpy(out,in);
      break;
    case QUALITY_LABEL:
      quality_enum_to_string(in,figure.quality);
      strcpy(out,in);
      break;
    case CONFIDENCE_LABEL:
      confidence_enum_to_string(in,figure.confidence);
      strcpy(out,in);
      break;
    case FFLAGS_LABEL:
      flags_to_string(in,figure.flags);
      strcpy(out,in);
      break;
  }
  */
}

void draw_gui_elements(Unit *units)
{
  char content[256];
  for(int i=0;i<NUMARMYBUILDER_GUI_LABELS;i++)
  {
    GuiLabel(ARMYBUILDER_LABEL_RECTANGLES[i], ARMYBUILDER_LABEL_TITLE[i]);
  }
  for(int i=SECTION_START;i<SECTION_UNITS;i++)
  {
    get_label_content_units(i,content);
    GuiLabel(ARMYBUILDER_LABEL_RECTANGLES[i+NUMARMYBUILDER_GUI_LABELS], content);
  }
  for(int i=SECTION_UNITS;i<SECTION_UNIT;i++)
  {
    get_label_content_unit(units[0],i,content);
    GuiLabel(ARMYBUILDER_LABEL_RECTANGLES[i+NUMARMYBUILDER_GUI_LABELS], content);
  }

  Rectangle offset = {0,0,0,0};
  Rectangle out = {0,0,0,0};
  for(int j=0;j<units[0].figurecnt;j++)
  {
    for(int i=SECTION_UNIT;i<SECTION_FIGURE;i++)
    {
      out = RectangleAdd(offset,ARMYBUILDER_LABEL_RECTANGLES[i+NUMARMYBUILDER_GUI_LABELS]);
      get_label_content_figure(units[0].figures[j],i,content);
      GuiLabel(out,content);
    }
    offset = RectangleAdd(offset,ARMYBUILDER_LABEL_RECTANGLES[FIGURE_TABLE_OFFSET]);
    printf("x: %i, y: %i\n",offset.x,offset.y);
    printf("x: %i, y: %i\n",ARMYBUILDER_LABEL_RECTANGLES[FIGURE_TABLE_OFFSET].x,ARMYBUILDER_LABEL_RECTANGLES[FIGURE_TABLE_OFFSET].y);
  }
}
void run_armybuilder(Unit *units)
{
/*
int dropdowntextcnt = 2;
int dropdowntextactive = 0;
const char *dropdowntext[2] = {"RAKI","DAWRI"};
*/
  while(!WindowShouldClose())
  {
    BeginDrawing();
      ClearBackground(RAYWHITE);
      draw_gui_elements(units);
    EndDrawing();
  }
}

Rectangle RectangleAdd(Rectangle rec1,Rectangle rec2)
{
  Rectangle out;
  out.x = rec1.x+rec2.x;
  out.y = rec1.y+rec2.y;
  out.width = rec1.width+rec2.width;
  out.height = rec1.height+rec2.height;
  return out;
}
