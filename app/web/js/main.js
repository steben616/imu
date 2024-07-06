import * as THREE from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';
// Three.js uses a right-handed coordinate frame, where:
// positive x-axis(red) points to the right
// positive y-axis(green) points up and
// positive z-axis(blue) points towards the viewer. (not visible)
// rotate around x axis by 90 deg so that:
// y(green, not visible) is towards the viewer and z(blue) is down
let scene = new THREE.Scene();
scene.background = new THREE.Color(0x404040);
let camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000)

let renderer = new THREE.WebGLRenderer({antialias: true, gammaOutput: true, alpha: true});
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setAnimationLoop(animate);
document.body.appendChild(renderer.domElement);

const controls = new OrbitControls(camera, renderer.domElement);

const root = new THREE.Group();
const XZYMatrix = new THREE.Matrix4().set(
    0, 1, 0, 0,
    0, 0, 1, 0,
    1, 0, 0, 0,
    0, 0, 0, 1
);
root.applyMatrix4(XZYMatrix);
scene.add(root);

// grid
const grid = new THREE.GridHelper(10, 10)
grid.rotation.x = Math.PI / 2
root.add(grid);

// axes
const axesHelper = new THREE.AxesHelper(3);
root.add(axesHelper);

// cylinder
const geometry = new THREE.CylinderGeometry(0.01, 0.10, 3, 32, 1, true);
const material = new THREE.MeshNormalMaterial(); //new THREE.MeshBasicMaterial({ color: 0xA3D2E2 });
const cylinder = new THREE.Mesh(geometry, material);
root.add(cylinder);

camera.position.x = 6;
camera.position.y = 6;
camera.position.z = 6;
controls.update();

wsConnect();

function animate() {
    controls.update();
	renderer.render(scene, camera);
}

function wsConnect() {
    let ws = new WebSocket("ws://localhost:9002/");
    ws.onmessage = function(e) {
        console.log(e.data);
        let rq = JSON.parse(e.data)
        let quaternion = new THREE.Quaternion(rq.i, rq.j, rq.k, rq.r).normalize();
        cylinder.rotation.setFromQuaternion(quaternion, 'XYZ');
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