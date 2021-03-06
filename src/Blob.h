#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofEvents.h"
#include "videoElement.h"

#define MAX_HISTORY 10
#define NEIGHBOR_HISTORY 5
#define VELOCITY_HISTORY 5


// a blob saves its position history in TimedPoint
// time parameter not in use right now
class TimedPoint {
    public:
        ofPoint point;
        int time;

        void set(int x, int y) {
            this->point.set( x, y );
            this->time = ofGetUnixTime();
        }
};


class Pair {
    public:
        int blob1;
        int blob2;

        Pair(int p1, int p2) {
            blob1 = p1;
            blob2 = p2;
        }
};


class tinyNeighbor {
    public:
        ofPoint position;
        bool movingMean;
        bool onStage;

        void set(ofPoint p, bool mm, bool os) {
            this->position = p;
            this->movingMean = mm;
            this->onStage = os;
        }
};


// to compute the distance between a blob and a neighbor blob
// it keeps a copy of the distance over time, to judge if distance stays the same
class Neighbor {
    public:

//        Neighbor() { cout << "neighbor" << endl; };
//        ~Neighbor() { cout << "~neighbor" << endl; };
        int id;
        bool updated;
        vector<float> distance;
        bool steadyDistance;
        int steadyStart;
        int steadyTimer;
        bool steadyRewarded;
        bool movingMean;
        bool onStage;

        Neighbor() {
            steadyStart = 0;
            steadyTimer = 0;
            steadyRewarded = false;
            steadyDistance = false;
            updated = false;
            movingMean = true;
            onStage = true;
        }

        float getMean() {
            float sum = 0;
            for (std::vector<float>::iterator it = distance.begin(); it != distance.end(); ++it) {
                sum += *it;
            }
            return sum / distance.size();
        }

        float getVariance() {
            float mean = getMean();
            float tmp = 0;
            for (std::vector<float>::iterator it = distance.begin(); it != distance.end(); ++it) {
                tmp += (mean - *it) * (mean - *it);
            }
            return tmp / distance.size();
        }

        float getStdDev() {
            return sqrtf( getVariance() );
        }

        float getMedian() {
            std::vector<float> distArray = std::vector<float>(distance);
            std::sort(distArray.begin(), distArray.end());
            if (distArray.size() % 2 == 0) {
                return (distArray[(distArray.size()/2)-1] + distArray[distArray.size()/2] ) / 2.0;
            } else {
                return distArray[distArray.size()/2];
            }
        }
};


// Blob represents a detected human
// information from blobserver software (camera tracking)
class Blob {
    public:
        int id;

        // information coming from blobserver
        // ofVec2f
    	ofPoint _rawPos;
    	ofPoint position;
    	ofPoint velocity;
        float size;
        float age;
        float lostDuration;
        float vel;
        vector<float> velHistory;
        bool onStage;
        bool counted;

        vector<TimedPoint> rawHistory;
    	vector<TimedPoint> history;    // array length of MAX_HISTORY

        // internal information
        int lifetime;
        int maxLifetime;
        bool updated;	// updated by last sample ?

        // analysis
        bool frozen;            //
        bool properFreeze;      // after a threshold of time
        bool overFrozen;
        int frozenStart;
        int frozenTimer;
        bool lost;
        bool occluded;

        ofEvent<int> onLost;
        ofEvent<int> onCreate;
        ofEvent<int> onFreeze;
        ofEvent<int> unFreeze;
        ofEvent<int> overFreeze;
        ofEvent<int> prepareToDie;
        ofEvent<int> updatePosition;
        ofEvent<int> onEnterStage;
        ofEvent<int> onLeaveStage;
        ofEvent<Pair> onSteady;
        ofEvent<Pair> onSteadyReward;
        ofEvent<Pair> onBreakSteady;

        bool movingMean;
        std::map<int, Neighbor> neighbors;
        bool steadyRewarded;

        bool onEdge;    // if blob disappears, most likely bc it escaped the frame


        Blob();
        ~Blob();
        void init();
        void setRawPosition(float x, float y);
        void processRawPosition(float frameW = 500, float frameH = 500, float stageRadius = 200, int y_mean = 5);
        void setVelocity(float dx, float dy);
        void analyze(float freezeMinVel, float freezeMinTime, float freezeMaxTime, float movingThr);
        // void analyzeNeighbors(std::map<int, ofPoint> neighborLocation, float distStdDevThr, int steadyReward);
        void analyzeNeighbors(std::map<int, tinyNeighbor> neighborLocation, float distStdDevThr, float minSteadyDistance);
        ofPoint transformPerspective(ofPoint& v);
        void update(int minLostTime);
        void updateVideo();
        bool isAlive();

        cv::Mat* perspectiveMat;

        ofPtr<mediaElement> mediaLink;
        std::map<int, ofPtr<mediaElement> > bridgeLink;
        bool videoTrace;

};
