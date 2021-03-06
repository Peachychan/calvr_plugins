#include "ModelHandler.h"

using namespace cvr;
using namespace osg;
using namespace std;

namespace ElevatorRoom
{


ModelHandler::ModelHandler()
{
    _activeObject = NULL;
    _geoRoot = new osg::MatrixTransform();
    _crosshairPat = NULL;
    _scoreText = NULL;

    _dataDir = ConfigManager::getEntry("Plugin.ElevatorRoom.DataDir");
    _dataDir = _dataDir + "/";

    _loaded = false;
    _doorDist = 0;
    _activeDoor = 0;
    _lightColor = 0;
    _doorInView = false;
}

ModelHandler::~ModelHandler()
{

}

void ModelHandler::update()
{
    osg::Vec3 pos, headpos, handdir;
    headpos = PluginHelper::getHeadMat(0).getTrans();
    handdir = osg::Vec3(0,1,0) * PluginHelper::getHandMat(0);
    osg::Matrixd rotMat;
    rotMat = PluginHelper::getHandMat(0);
    rotMat.setTrans(osg::Vec3(0,0,0));
    pos = PluginHelper::getHeadMat(0).getTrans() + (osg::Vec3(-32, 200, 0) * rotMat);

    _crosshairPat->setPosition(pos); 



    float angle = 0, tolerance = 15 * (M_PI / 180), roomRad = 6;
    osg::Vec3 headForward, headToDoor;
    headForward = osg::Vec3(1, 0, 0) * PluginHelper::getObjectMatrix();

    Vec3 lightPos = osg::Quat(_activeDoor * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, roomRad, 0.0);
    headToDoor = lightPos - (osg::Vec3(0,0,0) * PluginHelper::getHeadMat());

    headForward.normalize();
    headToDoor.normalize();

    angle = acos(headForward * headToDoor);
    angle -= M_PI;
    if (0)//angle < 0) 
        angle *= -1;

    if (angle < tolerance && !_doorInView)
    {
        _doorInView = true;
        std::cout << "Door entering view" << std::endl;
    }
    else if (angle > tolerance && _doorInView)
    {
        std::cout << "Door leaving view" << std::endl;
        _doorInView = false;
    }
}

void ModelHandler::loadModels(osg::MatrixTransform * root)
{
    if (root && _geoRoot)
    {
        root->addChild(_geoRoot.get());
    }

    std::string _wallTex, _floorTex, _ceilingTex, _doorTex,
            _alienTex, _allyTex, _checkTex1, _checkTex2, _elevTex;

    _wallTex = ConfigManager::getEntry("Plugin.ElevatorRoom.WallTexture");
    _floorTex = ConfigManager::getEntry("Plugin.ElevatorRoom.FloorTexture");
    _ceilingTex = ConfigManager::getEntry("Plugin.ElevatorRoom.CeilingTexture");
    _doorTex = ConfigManager::getEntry("Plugin.ElevatorRoom.DoorTexture");
    _elevTex = ConfigManager::getEntry("Plugin.ElevatorRoom.ElevatorTexture");

    _alienTex = ConfigManager::getEntry("Plugin.ElevatorRoom.AlienTexture");
    _allyTex = ConfigManager::getEntry("Plugin.ElevatorRoom.AllyTexture");

    _checkTex1 = ConfigManager::getEntry("Plugin.ElevatorRoom.CheckerTexture1");
    _checkTex2 = ConfigManager::getEntry("Plugin.ElevatorRoom.CheckerTexture2");


    osg::Vec4 grey     = osg::Vec4(0.7, 0.7, 0.7, 1.0),
              brown    = osg::Vec4(0.3, 0.15, 0.0, 1.0),
              darkgrey = osg::Vec4(0.3, 0.3, 0.3, 1.0),
              red      = osg::Vec4(1, 0, 0, 1), 
              blue     = osg::Vec4(0, 0, 1, 1),
              green    = osg::Vec4(0, 1, 0, 1),
              yellow   = osg::Vec4(1, 1, 0, 1),
              orange   = osg::Vec4(1, 0.5, 0, 1),
              white    = osg::Vec4(1, 1, 1, 1);

    float roomRad = 6.0, angle = 2 * M_PI / NUM_DOORS;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat;
    osg::ref_ptr<osg::ShapeDrawable> drawable;
    osg::ref_ptr<osg::Geode> geode;
    osg::ref_ptr<osg::Geometry> geo;

    osg::ref_ptr<osg::Texture2D> tex;
    osg::ref_ptr<osg::Image> img;
    osg::ref_ptr<osg::StateSet> state;


    // Lights
    osg::ref_ptr<osg::Sphere> shape = new osg::Sphere(osg::Vec3(0,-4.75, 4.0), 0.2);
    for (int i = 0; i < NUM_DOORS; ++i)
    {
        pat = new osg::PositionAttitudeTransform();
        drawable = new osg::ShapeDrawable(shape);
        drawable->setColor(osg::Vec4(0.5, 0.5, 0.5, 1.0));
        geode = new osg::Geode();
        geode->addDrawable(drawable);

        osg::ref_ptr<osg::ShapeDrawable> redDrawable, greenDrawable, yellowDrawable,
            orangeDrawable, whiteDrawable;
        osg::ref_ptr<osg::Geode> redGeode, greenGeode, yellowGeode, orangeGeode,
            whiteGeode;

        redDrawable = new osg::ShapeDrawable(shape);
        redDrawable->setColor(red);
        redGeode = new osg::Geode();
        redGeode->addDrawable(redDrawable);

        greenDrawable = new osg::ShapeDrawable(shape);
        greenDrawable->setColor(green);
        greenGeode = new osg::Geode();
        greenGeode->addDrawable(greenDrawable);

        yellowDrawable = new osg::ShapeDrawable(shape);
        yellowDrawable->setColor(yellow);
        yellowGeode = new osg::Geode();
        yellowGeode->addDrawable(yellowDrawable);

        orangeDrawable = new osg::ShapeDrawable(shape);
        orangeDrawable->setColor(orange);
        orangeGeode = new osg::Geode();
        orangeGeode->addDrawable(orangeDrawable);

        whiteDrawable = new osg::ShapeDrawable(shape);
        whiteDrawable->setColor(white);
        whiteGeode = new osg::Geode();
        whiteGeode->addDrawable(whiteDrawable);

        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0.0, 0.0, 1.0)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));

        osg::ref_ptr<osg::Switch> switchNode;
        switchNode = new osg::Switch();
        
        switchNode->addChild(geode, true);
        switchNode->addChild(redGeode, false);
        switchNode->addChild(greenGeode, false);
        switchNode->addChild(yellowGeode, false);
        switchNode->addChild(orangeGeode, false);
        switchNode->addChild(whiteGeode, false);

        pat->addChild(switchNode);
        _geoRoot->addChild(pat);

        _lightSwitch.push_back(switchNode);
        _lights.push_back(drawable);
        
        
        // Sound
        
        osg::Vec3 pos, center, dir;
        osg::Matrix o2w, local2o;
        o2w = PluginHelper::getObjectMatrix();
        local2o = _geoRoot->getInverseMatrix();

        pos = osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0);
        pos = pos * local2o * o2w;
        center = _geoRoot->getMatrix().getTrans();
        center = center * local2o * o2w;
        dir = pos - center;

        // 1 - 8 ding sounds
/*        
        if (_audioHandler)
        {
            _audioHandler->loadSound(i + DING_OFFSET, dir, pos);
        }
*/
    }


    // Aliens 
    geode = new osg::Geode();
    geo = drawBox(osg::Vec3(0, -6, 0.5), 1.0, 0.01, 1.0, grey);
    geode->addDrawable(geo);

    osg::ref_ptr<osg::Geode> redGeode = new Geode();
    geo = drawBox(osg::Vec3(0, -6, 0.5), 1.0, 0.01, 1.0, red);
    redGeode->addDrawable(geo);

    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _alienTex);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = geode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    state = redGeode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    for (int i = 0; i < NUM_DOORS; ++i)
    {
        pat = new osg::PositionAttitudeTransform();
        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0, 0, 1)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));
        pat->setScale(osg::Vec3(1, 1, 2));
        
        osg::ref_ptr<osg::Switch> switchNode;
        switchNode = new osg::Switch();
        
        switchNode->addChild(geode, false);
        switchNode->addChild(redGeode, false);
        
        pat->addChild(switchNode);
        _geoRoot->addChild(pat);

        _aliensSwitch.push_back(switchNode);


        // Sound
        
        osg::Vec3 pos = osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0);
        osg::Vec3 dir = pos - osg::Vec3(0,0,0);

        // 9 - 16 explosion sounds
   /*
        if (_audioHandler)
        {
            _audioHandler->loadSound(i + EXPLOSION_OFFSET, dir, pos);
        }
   */
    }   


    // Allies 
    geode = new osg::Geode();
    geo = drawBox(osg::Vec3(0, -6, 0.5), 1.0, 0.01, 1.0, grey);
    geode->addDrawable(geo);
    
    redGeode = new osg::Geode();
    geo = drawBox(osg::Vec3(0, -6, 0.5), 1.0, 0.01, 1.0, red);
    redGeode->addDrawable(geo);

    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _allyTex);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = geode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    state = redGeode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    for (int i = 0; i < NUM_DOORS; ++i)
    {
        pat = new osg::PositionAttitudeTransform();
        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0, 0, 1)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));
        pat->setScale(osg::Vec3(1, 1, 2));

        osg::ref_ptr<osg::Switch> switchNode;
        switchNode = new osg::Switch();
        
        switchNode->addChild(geode, false);
        switchNode->addChild(redGeode, false);
        
        pat->addChild(switchNode);
        _geoRoot->addChild(pat);

        _alliesSwitch.push_back(switchNode);
    }   


    // Checkerboards 
    geode = new osg::Geode();
    geo = drawBox(osg::Vec3(0, -6, 0.5), 1.0, 0.01, 1.0, grey);
    geode->addDrawable(geo);

    osg::ref_ptr<osg::Geode> geode2 = new osg::Geode();
    geode2->addDrawable(geo);

    // texture 1
    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _checkTex1);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }
    state = geode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    // texture 2
    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _checkTex2);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = geode2->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    for (int i = 0; i < NUM_DOORS; ++i)
    {
        pat = new osg::PositionAttitudeTransform();
        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0, 0, 1)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));
        pat->setScale(osg::Vec3(1, 1, 2));

        osg::ref_ptr<osg::Switch> switchNode;
        switchNode = new osg::Switch();
        
        switchNode->addChild(geode, false);
        switchNode->addChild(geode2, false);
        
        pat->addChild(switchNode);
        _geoRoot->addChild(pat);

        _checkersSwitch.push_back(switchNode);
    }   

    // Walls
    
    geode = new osg::Geode();
    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _wallTex);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = geode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    
    float wallTexScale = 0.5;

    // Left front
    geo = drawBox(osg::Vec3(3.0, -5.0, 1.0), 3.25, 0.5, 4.0, grey, wallTexScale); 
    geode->addDrawable(geo);
    
    // Right front
    geo = drawBox(osg::Vec3(-3.0, -5.0, 1.0), 3.25, 0.5, 4.0, grey, wallTexScale);
    geode->addDrawable(geo);

    // Top
    geo = drawBox(osg::Vec3(0.0, -5.0, 4.5), 9.0, 0.5, 3.0, grey, wallTexScale);
    geode->addDrawable(geo);

    
    // Elevator
    
    osg::ref_ptr<osg::Geode> elevatorGeode = new osg::Geode();
    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _elevTex);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = elevatorGeode->getOrCreateStateSet();
	state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);


    // Left side 
    geo = drawBox(osg::Vec3( 1.25, -7.0, 1.5), 0.5, 4.0, 5.0, grey);
    elevatorGeode->addDrawable(geo);

    // Right side 
    geo = drawBox(osg::Vec3(-1.25, -7.0, 1.5), 0.5, 4.0, 5.0, grey);
    elevatorGeode->addDrawable(geo);

    // Back
    geo = drawBox(osg::Vec3(0.0, -9.25, 1.5), 3.0, 0.5, 5.0, grey);
    elevatorGeode->addDrawable(geo);

    // Elevator floor
    geo = drawBox(osg::Vec3(0.0, -7.0, -1.15), 3.0, 3.75, 0.5, grey);
    elevatorGeode->addDrawable(geo);

    for (int i = 0; i < NUM_DOORS; ++i)
    {
        pat = new osg::PositionAttitudeTransform();
        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0, 0, 1)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));
        pat->addChild(geode);
        pat->addChild(elevatorGeode);
        _geoRoot->addChild(pat);
    }


    // Ceiling 
    
    geode = new osg::Geode();

    geo = drawBox(osg::Vec3(0.0, 0.0, 5.0), 40.0, 40.0, 0.1, grey);
    geode->addDrawable(geo);

    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _ceilingTex);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = geode->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
    state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    _geoRoot->addChild(geode);
    

    // Floor

    geode = new osg::Geode();

    geo = drawBox(osg::Vec3(0.0, 0.0, -1.0), 40.0, 40.0, 0.1, grey);
    geode->addDrawable(geo);

    tex = new osg::Texture2D();
    img = osgDB::readImageFile(_dataDir + _floorTex);
    if (img)
    {
        tex->setImage(img);
        tex->setResizeNonPowerOfTwoHint(false);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    }

    state = geode->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
    state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);   

    _geoRoot->addChild(geode);


    // Doors
    
    osg::ref_ptr<osg::Geometry> ldoorGeo, rdoorGeo;

    ldoorGeo = drawBox(osg::Vec3( 0.75, -5.2, 1.0), 1.5, 0.5, 4.0, grey);
    rdoorGeo = drawBox(osg::Vec3(-0.75, -5.2, 1.0), 1.5, 0.5, 4.0, grey);

    for (int i = 0; i < NUM_DOORS; ++i)
    {
        pat = new osg::PositionAttitudeTransform();
        geode = new osg::Geode();

        tex = new osg::Texture2D();
        img = osgDB::readImageFile(_dataDir + _doorTex);
        if (img)
        {
            tex->setImage(img);
            tex->setResizeNonPowerOfTwoHint(false);
            tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
            tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
        }

        state = geode->getOrCreateStateSet();
        state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
        state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        geode->addDrawable(rdoorGeo);
        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0, 0, 1)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));
        pat->addChild(geode);
        _rightdoorPat.push_back(pat);
        _geoRoot->addChild(pat);


        pat = new osg::PositionAttitudeTransform();
        geode = new osg::Geode();

        state = geode->getOrCreateStateSet();
        state->setTextureAttributeAndModes(0,tex,osg::StateAttribute::ON);
        state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        geode->addDrawable(ldoorGeo);
        pat->setAttitude(osg::Quat(i * angle, osg::Vec3(0, 0, 1)));
        pat->setPosition(osg::Quat(i * angle, osg::Vec3(0, 0, 1)) * osg::Vec3(0.0, -roomRad, 0.0));
        pat->addChild(geode);
        _leftdoorPat.push_back(pat);
        _geoRoot->addChild(pat);
    }


    // Score text
    
    osg::Vec3 pos = osg::Vec3(-500, 0, 300);
    _scoreText = new osgText::Text();
    _scoreText->setText("Score: 0");
    _scoreText->setCharacterSize(20);
    _scoreText->setAlignment(osgText::Text::LEFT_CENTER);
    _scoreText->setPosition(pos);
    _scoreText->setColor(osg::Vec4(1,1,1,1));
    _scoreText->setBackdropColor(osg::Vec4(0,0,0,0));
    _scoreText->setAxisAlignment(osgText::Text::XZ_PLANE);

    float width = 200, height = 50;
    osg::ref_ptr<osg::Geometry> quad = makeQuad(width, height, 
        osg::Vec4(1.0,1.0,1.0,0.5), pos - osg::Vec3(10, 0, 25));

    pat = new osg::PositionAttitudeTransform();
    geode = new osg::Geode();

    geode->addDrawable(_scoreText);
    geode->addDrawable(quad);

    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    pat->addChild(geode);
    PluginHelper::getScene()->addChild(pat);


    // Crosshair

    width = 4;
    height = 0.3;
    pos = osg::Vec3(0, -2500, 0);
    pos = osg::Vec3(-25, 200, 0) + PluginHelper::getHeadMat().getTrans();
    osg::Vec4 color(0.8, 0.0, 0.0, 1.0);
    osg::Geode *chGeode = new osg::Geode();
    _crosshairPat = new osg::PositionAttitudeTransform();
    _crosshairPat->setPosition(pos);
    pos = osg::Vec3(0,0,0);

    // horizontal
    quad = makeQuad(width, height, color, pos - osg::Vec3(width/2, 0, height/2));
    chGeode->addDrawable(quad);

    // vertical
    quad = makeQuad(height, width, color, pos - osg::Vec3(height/2, 0, width/2));
    chGeode->addDrawable(quad);

    chGeode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    _crosshairPat->addChild(chGeode); 

    if (ConfigManager::getEntry("Plugin.ElevatorRoom.Crosshair") == "on" )
    {
        PluginHelper::getScene()->addChild(_crosshairPat);
    }

    _loaded = true;
}

osg::ref_ptr<osg::Geometry> ModelHandler::drawBox(osg::Vec3 center, float x, 
    float y, float z, osg::Vec4 color, float texScale)
{
    osg::ref_ptr<osg::Vec3Array> verts;
    osg::ref_ptr<osg::Vec4Array> colors;
    osg::ref_ptr<osg::Vec3Array> normals;
    osg::ref_ptr<osg::DrawArrays> primitive;
    osg::ref_ptr<osg::Geometry> geometry;
    osg::ref_ptr<osg::Vec2Array> texcoords;

    osg::Vec3 up;
    osg::Vec3 down;
    osg::Vec3 left; 
    osg::Vec3 right; 
    osg::Vec3 front; 
    osg::Vec3 back;

    float xMin = center[0] - x/2, xMax = center[0] + x/2,
          yMin = center[1] - y/2, yMax = center[1] + y/2,
          zMin = center[2] - z/2, zMax = center[2] + z/2;

    up    = osg::Vec3(0, 0,  1);
    down  = osg::Vec3(0, 0, -1);
    left  = osg::Vec3(-1, 0, 0);
    right = osg::Vec3(-1, 0, 0);
    front = osg::Vec3(0, -1, 0);
    back  = osg::Vec3(0,  1, 0);

    verts = new osg::Vec3Array(0);
    colors = new osg::Vec4Array(1);
    normals = new osg::Vec3Array(0);
    primitive = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 0);
    geometry = new osg::Geometry();
    texcoords = new osg::Vec2Array(0);

    (*colors)[0] = color;

    geometry->setVertexArray(verts.get());
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    geometry->setNormalArray(normals.get());
    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->setUseDisplayList(false);
    geometry->addPrimitiveSet(primitive.get());
    geometry->setTexCoordArray(0, texcoords);
    
    x = x * texScale;
    y = y * texScale;
    z = z * texScale;

    // x y 
    texcoords->push_back(osg::Vec2(0.0, 0.0));
    texcoords->push_back(osg::Vec2(x,   0.0));
    texcoords->push_back(osg::Vec2(x,   y));
    texcoords->push_back(osg::Vec2(0.0, y));
    
    texcoords->push_back(osg::Vec2(0.0, 0.0));
    texcoords->push_back(osg::Vec2(y,   0.0));
    texcoords->push_back(osg::Vec2(y,   x));
    texcoords->push_back(osg::Vec2(0.0, x));
    
    // x z
    texcoords->push_back(osg::Vec2(0.0, z));
    texcoords->push_back(osg::Vec2(0.0, 0.0));
    texcoords->push_back(osg::Vec2(x,   0.0));
    texcoords->push_back(osg::Vec2(x  , z));

    texcoords->push_back(osg::Vec2(0.0, z));
    texcoords->push_back(osg::Vec2(0.0, 0.0));
    texcoords->push_back(osg::Vec2(x,   0.0));
    texcoords->push_back(osg::Vec2(x  , z));
   
    // y z
    texcoords->push_back(osg::Vec2(0.0, 0.0));
    texcoords->push_back(osg::Vec2(z,   0.0));
    texcoords->push_back(osg::Vec2(z,   y));
    texcoords->push_back(osg::Vec2(0.0, y));

    texcoords->push_back(osg::Vec2(0.0, 0.0));
    texcoords->push_back(osg::Vec2(z,   0.0));
    texcoords->push_back(osg::Vec2(z,   y));
    texcoords->push_back(osg::Vec2(0.0, y));

    // top 
    verts->push_back(osg::Vec3(xMax, yMax, zMax)); 
    normals->push_back(up);
    verts->push_back(osg::Vec3(xMin, yMax, zMax));
    normals->push_back(up);
    verts->push_back(osg::Vec3(xMin, yMin, zMax));
    normals->push_back(up);
    verts->push_back(osg::Vec3(xMax, yMin, zMax));
    normals->push_back(up);

    // bottom 
    verts->push_back(osg::Vec3(xMax, yMax, zMin));
    normals->push_back(down);
    verts->push_back(osg::Vec3(xMin, yMax, zMin));
    normals->push_back(down);
    verts->push_back(osg::Vec3(xMin, yMin, zMin));
    normals->push_back(down);
    verts->push_back(osg::Vec3(xMax, yMin, zMin));
    normals->push_back(down);

    // front 
    verts->push_back(osg::Vec3(xMin, yMin, zMax));
    normals->push_back(front);
    verts->push_back(osg::Vec3(xMin, yMin, zMin));
    normals->push_back(front);
    verts->push_back(osg::Vec3(xMax, yMin, zMin));
    normals->push_back(front);
    verts->push_back(osg::Vec3(xMax, yMin, zMax));
    normals->push_back(front);

    // back 
    verts->push_back(osg::Vec3(xMax, yMax, zMax));
    normals->push_back(back);
    verts->push_back(osg::Vec3(xMax, yMax, zMin));
    normals->push_back(back);
    verts->push_back(osg::Vec3(xMin, yMax, zMin));
    normals->push_back(back);
    verts->push_back(osg::Vec3(xMin, yMax, zMax));
    normals->push_back(back);

    // left 
    verts->push_back(osg::Vec3(xMin, yMax, zMax));
    normals->push_back(left);
    verts->push_back(osg::Vec3(xMin, yMax, zMin));
    normals->push_back(left);
    verts->push_back(osg::Vec3(xMin, yMin, zMin));
    normals->push_back(left);
    verts->push_back(osg::Vec3(xMin, yMin, zMax));
    normals->push_back(left);

    // right 
    verts->push_back(osg::Vec3(xMax, yMax, zMax));
    normals->push_back(right);
    verts->push_back(osg::Vec3(xMax, yMax, zMin));
    normals->push_back(right);
    verts->push_back(osg::Vec3(xMax, yMin, zMin));
    normals->push_back(right);
    verts->push_back(osg::Vec3(xMax, yMin, zMax));
    normals->push_back(right);

    primitive->setCount(4 * 6);
    geometry->dirtyBound();
    
    return geometry;
}

osg::ref_ptr<osg::Geometry> ModelHandler::makeQuad(float width, float height,
        osg::Vec4 color, osg::Vec3 pos)
{
    osg::Geometry * geo = new osg::Geometry();
    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->push_back(pos);
    verts->push_back(pos + osg::Vec3(width,0,0));
    verts->push_back(pos + osg::Vec3(width,0,height));
    verts->push_back(pos + osg::Vec3(0,0,height));

    geo->setVertexArray(verts);

    osg::DrawElementsUInt * ele = new osg::DrawElementsUInt(
            osg::PrimitiveSet::QUADS,0);

    ele->push_back(0);
    ele->push_back(1);
    ele->push_back(2);
    ele->push_back(3);
    geo->addPrimitiveSet(ele);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);

    osg::TemplateIndexArray<unsigned int,osg::Array::UIntArrayType,4,4> *colorIndexArray;
    colorIndexArray = new osg::TemplateIndexArray<unsigned int,
            osg::Array::UIntArrayType,4,4>;
    colorIndexArray->push_back(0);
    colorIndexArray->push_back(0);
    colorIndexArray->push_back(0);
    colorIndexArray->push_back(0);

    geo->setColorArray(colors);
    geo->setColorIndices(colorIndexArray);
    geo->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::Vec2Array* texcoords = new osg::Vec2Array;
    texcoords->push_back(osg::Vec2(0,0));
    texcoords->push_back(osg::Vec2(1,0));
    texcoords->push_back(osg::Vec2(1,1));
    texcoords->push_back(osg::Vec2(0,1));
    geo->setTexCoordArray(0,texcoords);

    return geo;
}

void ModelHandler::openDoor()
{
    if (_activeDoor < 0 || _activeDoor >= (int)_leftdoorPat.size() || _activeDoor >= (int)_rightdoorPat.size())
    {
        return;
    }

    osg::PositionAttitudeTransform *lpat, *rpat;
    lpat = _leftdoorPat[_activeDoor];
    rpat = _rightdoorPat[_activeDoor];

    lpat->setPosition(lpat->getPosition() + lpat->getAttitude() * osg::Vec3(DOOR_SPEED,0,0));
    rpat->setPosition(rpat->getPosition() + rpat->getAttitude() * osg::Vec3(-DOOR_SPEED,0,0));

    _doorDist += DOOR_SPEED;
}

void ModelHandler::closeDoor()
{
    if (_activeDoor < 0 || _activeDoor >= (int)_leftdoorPat.size() || _activeDoor >= (int)_rightdoorPat.size())
    {
        return;
    }
    
    osg::PositionAttitudeTransform *lpat, *rpat;
    lpat = _leftdoorPat[_activeDoor];
    rpat = _rightdoorPat[_activeDoor];

    lpat->setPosition(lpat->getPosition() + lpat->getAttitude() * osg::Vec3(-DOOR_SPEED,0,0));
    rpat->setPosition(rpat->getPosition() + rpat->getAttitude() * osg::Vec3(DOOR_SPEED,0,0));

    _doorDist -= DOOR_SPEED;
}

void ModelHandler::setMode(Mode mode)
{
    _alliesSwitch[_activeDoor]->setAllChildrenOff();
    _aliensSwitch[_activeDoor]->setAllChildrenOff();
    _checkersSwitch[_activeDoor]->setAllChildrenOff();
    
    _mode = mode;
    if (mode == ALLY)
    {
        _alliesSwitch[_activeDoor]->setSingleChildOn(0);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_alliesSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }

    else if (mode == ALIEN)
    {
        _aliensSwitch[_activeDoor]->setSingleChildOn(0);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_aliensSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }

    else if (mode == CHECKER)
    {
        _checkersSwitch[_activeDoor]->setSingleChildOn(0);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_checkersSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }

    _lightColor = _lightSwitch[_activeDoor]->getNumChildren() - 1;
    _lightSwitch[_activeDoor]->setValue((int)_lightColor, true);
}

osg::ref_ptr<osg::Geode> ModelHandler::getActiveObject()
{
    return _activeObject;
}

void ModelHandler::setScore(int score)
{
    char buf[10];
    sprintf(buf, "%d", score);
    std::string text = "Score: ";
    text += buf;
    _scoreText->setText(text);
}

void ModelHandler::flashActiveLight()
{
    if (_lightSwitch[_activeDoor]->getValue(_lightColor))
    {
        _lightSwitch[_activeDoor]->setSingleChildOn(NONE);
    }
    else
    {
        _lightSwitch[_activeDoor]->setSingleChildOn(_lightColor);
    }
}

void ModelHandler::flashCheckers()
{
    if (_checkersSwitch[_activeDoor]->getValue(0))
    {
        _checkersSwitch[_activeDoor]->setValue(0, false);
        _checkersSwitch[_activeDoor]->setValue(1, true);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_checkersSwitch[_activeDoor]->getChild(1));
        if (geode)
        {
            _activeObject = geode;
        }
    }
    else
    {
        _checkersSwitch[_activeDoor]->setValue(0, true);
        _checkersSwitch[_activeDoor]->setValue(1, false);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_checkersSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }
}

void ModelHandler::flashAlien()
{
    if (_aliensSwitch[_activeDoor]->getValue(0))
    {
        _aliensSwitch[_activeDoor]->setValue(0, false);
        _aliensSwitch[_activeDoor]->setValue(1, true);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_aliensSwitch[_activeDoor]->getChild(1));
        if (geode)
        {
            _activeObject = geode;
        }
    }
    else
    {
        _aliensSwitch[_activeDoor]->setValue(0, true);
        _aliensSwitch[_activeDoor]->setValue(1, false);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_aliensSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }
}

void ModelHandler::flashAlly()
{
    if (_alliesSwitch[_activeDoor]->getValue(0))
    {
        _alliesSwitch[_activeDoor]->setValue(0, false);
        _alliesSwitch[_activeDoor]->setValue(1, true);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_alliesSwitch[_activeDoor]->getChild(1));
        if (geode)
        {
            _activeObject = geode;
        }
    }
    else
    {
        _alliesSwitch[_activeDoor]->setValue(0, true);
        _alliesSwitch[_activeDoor]->setValue(1, false);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_alliesSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }

    }
}

void ModelHandler::setActiveDoor(int doorNum)
{
    _activeDoor = doorNum;

    if (_mode == ALLY)
    {
        _alliesSwitch[_activeDoor]->setSingleChildOn(0);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_alliesSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }

    else if (_mode == ALIEN)
    {
        _aliensSwitch[_activeDoor]->setSingleChildOn(0);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_aliensSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }

    else if (_mode == CHECKER)
    {
        _checkersSwitch[_activeDoor]->setSingleChildOn(0);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_checkersSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }


}

void ModelHandler::setAlien(bool val)
{
    _aliensSwitch[_activeDoor]->setValue(0, val);
    _aliensSwitch[_activeDoor]->setValue(1, val);

    if (val) // only show one child
    {
        _aliensSwitch[_activeDoor]->setValue(1, false);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_aliensSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }
}

void ModelHandler::setAlly(bool val)
{
    _alliesSwitch[_activeDoor]->setValue(0, val);
    _alliesSwitch[_activeDoor]->setValue(1, val);

    if (val) // only show one child
    {
        _alliesSwitch[_activeDoor]->setValue(1, false);

        osg::Geode * geode;
        geode = dynamic_cast<osg::Geode *>(_alliesSwitch[_activeDoor]->getChild(0));
        if (geode)
        {
            _activeObject = geode;
        }
    }
}

void ModelHandler::setLight(bool val)
{
    _lightColor = (int)_mode;
    if (val)
    {
        _lightSwitch[_activeDoor]->setValue(0, false);
        _lightSwitch[_activeDoor]->setValue(_lightColor, true);
    }
    else
    {
        _lightSwitch[_activeDoor]->setValue(0, true);
        _lightSwitch[_activeDoor]->setValue(_lightColor, false);
    }
}

float ModelHandler::getDoorDistance()
{
    return _doorDist;
}

bool ModelHandler::doorInView()
{
    return _doorInView;
}

};

