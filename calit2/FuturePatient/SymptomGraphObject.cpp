#include "SymptomGraphObject.h"

#include <cvrConfig/ConfigManager.h>
#include <cvrKernel/ComController.h>
#include <cvrInput/TrackingManager.h>
#include <cvrUtil/OsgMath.h>

#include <iostream>
#include <sstream>

using namespace cvr;

SymptomGraphObject::SymptomGraphObject(mysqlpp::Connection * conn, float width, float height, std::string name, bool navigation, bool movable, bool clip, bool contextMenu, bool showBounds) : LayoutTypeObject(name,navigation,movable,clip,contextMenu,showBounds)
{
    _conn = conn;
    _graph = new TimeRangeDataGraph();
    _graph->setDisplaySize(width,height);

    addChild(_graph->getGraphRoot());

    /*_intensityLabels[1] = "Mild";
    _intensityLabels[2] = "Moderate";
    _intensityLabels[3] = "Moderate to Severe";
    _intensityLabels[4] = "Severe";
    _intensityLabels[5] = "Fatal";*/

    _intensityLabels[1] = "Mild";
    _intensityLabels[2] = "Moderate";
    _intensityLabels[3] = "Moderate to Severe";
    _intensityLabels[4] = "Severe";
    _intensityLabels[5] = "Very Severe";

    _graph->setValueLabelMap(_intensityLabels);


    setBoundsCalcMode(SceneObject::MANUAL);
    osg::BoundingBox bb(-(width*0.5),-2,-(height*0.5),width*0.5,0,height*0.5);
    setBoundingBox(bb);

    _desktopMode = ConfigManager::getBool("Plugin.FuturePatient.DesktopMode",false);
}

SymptomGraphObject::~SymptomGraphObject()
{
}

bool SymptomGraphObject::addGraph(std::string name)
{

    struct timeRange
    {
	time_t start;
	time_t end;
	int intensity;
    };

    int numRanges = 0;
    struct timeRange * ranges = NULL;

    if(ComController::instance()->isMaster())
    {
	if(_conn)
	{
	    std::stringstream qss;
	    qss << "select unix_timestamp(start_timestamp) as start, unix_timestamp(end_timestamp) as end, intensity from Event where patient_id = \"1\" and name = \"" << name << "\";";

	    mysqlpp::Query query = _conn->query(qss.str().c_str());
	    mysqlpp::StoreQueryResult result = query.store();

	    numRanges = result.num_rows();
	    if(numRanges)
	    {
		ranges = new struct timeRange[numRanges];
		for(int i = 0; i < numRanges; ++i)
		{
		    ranges[i].start = atol(result[i]["start"].c_str());
		    ranges[i].end = atol(result[i]["end"].c_str());
		    ranges[i].intensity = atoi(result[i]["intensity"].c_str());
		}
	    }
	}

	ComController::instance()->sendSlaves(&numRanges,sizeof(int));
	if(numRanges)
	{
	    ComController::instance()->sendSlaves(ranges,numRanges*sizeof(struct timeRange));
	}
    }
    else
    {
	ComController::instance()->readMaster(&numRanges,sizeof(int));
	if(numRanges)
	{
	    ranges = new struct timeRange[numRanges];
	    ComController::instance()->readMaster(ranges,numRanges*sizeof(struct timeRange));
	}
    }

    if(numRanges)
    {
	std::vector<std::pair<time_t,time_t> > rangeList;
	std::vector<int> intensityList;

	for(int i = 0; i < numRanges; ++i)
	{
	    rangeList.push_back(std::pair<time_t,time_t>(ranges[i].start,ranges[i].end));
	    intensityList.push_back(ranges[i].intensity);
	}

	_graph->addGraph(name,rangeList,intensityList,5);

	delete[] ranges;

	return true;
    }
    else
    {
	std::cerr << "Warning: no entries for symptom: " << name << std::endl;
	return false;
    }
}

void SymptomGraphObject::setGraphSize(float width, float height)
{
    _graph->setDisplaySize(width,height);

    osg::BoundingBox bb(-(width*0.5),-2,-(height*0.5),width*0.5,0,height*0.5);
    setBoundingBox(bb);
}

void SymptomGraphObject::setBarVisible(bool vis)
{
    _graph->setBarVisible(vis);
}

float SymptomGraphObject::getBarPosition()
{
    return _graph->getBarPosition();
}

void SymptomGraphObject::setBarPosition(float pos)
{
    _graph->setBarPosition(pos);
}

bool SymptomGraphObject::getGraphSpacePoint(const osg::Matrix & mat, osg::Vec3 & point)
{
    osg::Matrix m;
    m = mat * getWorldToObjectMatrix();
    return _graph->getGraphSpacePoint(m,point);
}

void SymptomGraphObject::setGLScale(float scale)
{
    _graph->setGLScale(scale);
}

void SymptomGraphObject::setGraphDisplayRange(time_t start, time_t end)
{
    _graph->setDisplayRange(start,end);
}

void SymptomGraphObject::resetGraphDisplayRange()
{
    time_t start, end;
    start = _graph->getMinTimestamp();
    end = _graph->getMaxTimestamp();
    _graph->setDisplayRange(start,end);
}

void SymptomGraphObject::getGraphDisplayRange(time_t & start, time_t & end)
{
    _graph->getDisplayRange(start,end);
}

time_t SymptomGraphObject::getMaxTimestamp()
{
    return _graph->getMaxTimestamp();
}

time_t SymptomGraphObject::getMinTimestamp()
{
    return _graph->getMinTimestamp();
}

void SymptomGraphObject::updateCallback(int handID, const osg::Matrix & mat)
{
    if((_desktopMode && TrackingManager::instance()->getHandTrackerType(handID) == TrackerBase::MOUSE) ||
	(!_desktopMode && TrackingManager::instance()->getHandTrackerType(handID) == TrackerBase::POINTER))
    {
	osg::Vec3 start, end(0,1000,0);
	start = start * mat * getWorldToObjectMatrix();
	end = end * mat * getWorldToObjectMatrix();

	osg::Vec3 planePoint;
	osg::Vec3 planeNormal(0,-1,0);
	osg::Vec3 intersect;
	float w;

	if(linePlaneIntersectionRef(start,end,planePoint,planeNormal,intersect,w))
	{
	    _graph->setHover(intersect);
	}
    }
}

void SymptomGraphObject::leaveCallback(int handID)
{
    if((_desktopMode && TrackingManager::instance()->getHandTrackerType(handID) == TrackerBase::MOUSE) ||
	(!_desktopMode && TrackingManager::instance()->getHandTrackerType(handID) == TrackerBase::POINTER))
    {
	_graph->clearHoverText();
    }
}
