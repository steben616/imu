import * as THREE from 'three';
import { MTLLoader } from 'three/addons/loaders/MTLLoader.js';
import { OBJLoader } from 'three/addons/loaders/OBJLoader.js';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';
// Three.js uses a right-handed coordinate frame, where:
// positive x-axis(red) points to the right
// positive y-axis(green) points up and
// positive z-axis(blue) points towards the viewer. (not visible)
let scene = new THREE.Scene();
let camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000)

const ambientLight = new THREE.AmbientLight(0xcccccc, 0.6);
scene.add(ambientLight);
const pointLight = new THREE.PointLight(0xffffff, 1, 100);
pointLight.castShadow = true;
scene.add(pointLight);

let renderer = new THREE.WebGLRenderer({antialias: true, gammaOutput: true, alpha: true});
renderer.setPixelRatio( window.devicePixelRatio );
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setAnimationLoop(animate);
document.body.appendChild(renderer.domElement);

const controls = new OrbitControls(camera, renderer.domElement);

// rotate the root group such that
// x and y are planar and z is up
const root = new THREE.Group();
root.rotation.x = -1 * (Math.PI / 2);
root.rotation.z = (Math.PI );
scene.add(root);

// grid
const grid = new THREE.GridHelper(10, 10, 0x000000, 0x000000)
grid.rotation.x = Math.PI / 2
root.add(grid);

// axes
const axesHelper = new THREE.AxesHelper(3);
root.add(axesHelper);

// missile
let missile;
new MTLLoader()
    .setPath('models/missile/')
    .load('dummy.mtl', function(materials) {
        materials.preload();
        new OBJLoader()
            .setMaterials(materials)
            .setPath('models/missile/')
            .load('missile.obj', function(object) {
                object.scale.setScalar(0.30);
                var texture = new THREE.TextureLoader().load('models/missile/texture.png');
                object.traverse(function (child) {
                    if (child instanceof THREE.Mesh) {
                        child.material.map = texture;
                    }
                });
                root.add(object);
                missile = object;
            });
    });
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
        //console.log(e.data);
        let rq = JSON.parse(e.data)
        let quaternion = new THREE.Quaternion(rq.i, rq.j, rq.k, rq.r).normalize()
        // multiply by the default mapping quaternion
        quaternion.multiply(new THREE.Quaternion(0, 0, Math.sqrt(2)/2,  -1 * Math.sqrt(2)/2).normalize());
        missile.rotation.setFromQuaternion(quaternion, 'XYZ');
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