#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	_font.loadFont("Courier New", 40);

	//_ads.openLocal(851);
	_ads.openRemote("10.211.55.3.1.1", 851);
}

//--------------------------------------------------------------
void ofApp::update()
{
	_ads.writeValue<float>("MAIN.externalVar", ofGetElapsedTimef());
}

//--------------------------------------------------------------
void ofApp::draw()
{
	stringstream ss;
	ss << "counter: " << _ads.readValue<float>("MAIN.counter") << endl;
	ss << "test: " << _ads.readValue<float>("MAIN.test") << endl;

	_font.drawString(ss.str(), 100, 100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}