import * as THREE from 'three';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setAnimationLoop(animate);
document.body.appendChild(renderer.domElement);

// const geometry = new THREE.BoxGeometry( 1, 1, 1 );
// const material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } );
// const cube = new THREE.Mesh( geometry, material );
// scene.add( cube );

const axesHelper = new THREE.AxesHelper();
scene.add(axesHelper);
// Three.js uses a right-handed coordinate frame, where:
// positive x-axis(red) points to the right
// positive y-axis(green) points up and
// positive z-axis(blue) points towards the viewer. (not visible)
// rotate around x axis by 90 deg so that:
// y(green, not visible) is towards the viewer and z(blue) is down
axesHelper.rotation.x = Math.PI / 2;
axesHelper.rotation.y = 0;
axesHelper.rotation.z = 0;

camera.position.y = 2;
camera.position.z = 5;

wsConnect();

function animate() {
	renderer.render(scene, camera);
}

function wsConnect() {
    let ws = new WebSocket("ws://localhost:9002/");
    ws.onmessage = function(e) {
        let j = JSON.parse(e.data)
        axesHelper.rotation.z = j.y * 0.0174533;
        console.log(j);
    }
    ws.onopen = function(e) {
        console.log("onopen");
    };
    ws.onclose = function(e) {
        console.log("onclose");
    };
    ws.onerror = function(e) {
        console.log(e);
    };
}