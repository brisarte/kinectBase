#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;

	//Load shader
	shader.load( "vertexdummy.c", "kinectshader.c" );

	//Carrega o vídeo
	video.load("amazonia.mp4");
	video.play();

	fbo.allocate(kinect.width, kinect.height);
	fboVideo.allocate( video.getWidth(), video.getHeight());
}

//--------------------------------------------------------------
void ofApp::update() {
	
	ofBackground(0, 0, 0);
	
	kinect.update();
	
	// Só executa se aconteceu algo no kinect
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels());
		getNearMirror(grayImage, 158);
	}

	//Atualiza frame do video
	video.update();
	
}

void ofApp::getNearMirror(ofxCvGrayscaleImage &imgGray, int contrasteDistancia) {

	//imgGray.mirror(false,true);
	ofPixels & pixNoise = imgGray.getPixels();
	int numPixelsNoise = pixNoise.size();
	for (int i = 0; i < numPixelsNoise; i++) {
		pixNoise[i] = ofClamp(ofMap(pixNoise[i], 0, 255, -contrasteDistancia, 255), 0, 255); // Aumenta contraste de distancia
	}
	imgGray.flagImageChanged();
}

//--------------------------------------------------------------
void ofApp::draw() {
	
	ofSetColor(255, 255, 255);
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
		// draw from the live kinect
		kinect.drawDepth(10, 10, 200, 150);
		kinect.draw(210, 10, 200, 150);
		
		grayImage.draw(10, 160, 200, 150);
		
	}
	
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
        
    if(kinect.hasAccelControl()) {
        reportStream << "acc: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    }
    
	reportStream << "fps: " << ofGetFrameRate() << endl;
    
	ofDrawBitmapString(reportStream.str(), 20, 652);


	fbo.begin();
	
	ofSetColor( 255, 255, 255 );
	grayImage.draw(0, 0, kinect.width, kinect.height);

	fbo.end();

	/*
	fbo2.begin();
	
	ofSetColor( 255, 255, 255 );
	kinect.draw(0, 0, kinect.width, kinect.width);

	fbo2.end();

	*/

	//Enable shader
	shader.begin();

    //pass time value to shader
	//shader.setUniform1f("time", time );
			
	shader.setUniformTexture( "texture1", fbo.getTextureReference(), 1 ); //"1" means that it is texture 1

	//shader.setUniformTexture( "texture2", fbo2.getTextureReference(), 2 ); //"2" means that it is texture 2

	//Draw image through shader
	ofSetColor( 255, 255, 255 );
	kinect.draw( 210, 160, 200, 150);

	shader.end();


	fboVideo.begin();
	
	ofSetColor( 255, 255, 255 );
	grayImage.draw(0, 0, video.getWidth(), video.getHeight());

	fboVideo.end();

	//Enable shader
	shader.begin();
			
	shader.setUniformTexture( "texture1", fboVideo.getTextureReference(), 1 ); //"1" means that it is texture 1

	//Draw image through shader
	ofSetColor( 255, 255, 255 );
	video.draw( 0, 0, 1024, 768);

	shader.end();
    
}

void ofApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case ' ':
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			break;
			
		case '<':
		case ',':
			break;
			
		case '+':
		case '=':
			break;
			
		case '-':
			break;
			
		case 'w':
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}