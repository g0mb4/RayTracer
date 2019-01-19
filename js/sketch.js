var s1;
var mirror;
var particles = [];

var totalCollsisions;
var run_sim;

var GLOBL_LIFE = 5;
var GLOBL_DEATH_RADIUS = 350;

function setup() {
    var canvas = createCanvas(800, 600, WEBGL);
    canvas.parent('canvas');

    mirror = new Plane(1000, 1000, new Vector(0, 0, 1));
    s1 = new Sphere(new Vector(0, 0, 100), 100, 4/3);

    simRestart();
    run_sim = false;
}

function simRestart(){
    GLOBL_LIFE = parseInt(document.getElementById('life').value);
    GLOBL_DEATH_RADIUS = parseInt(document.getElementById('death_radius').value);

    var mesh_size = parseInt(document.getElementById('mesh_size').value);

    particles = [];
    if(mesh_size < 0){
        particles.push(new Particle(new Vector(-300, 0, 100), new Vector(1, 0, -0.5), GLOBL_LIFE, GLOBL_DEATH_RADIUS));
    } else {
        for(var y = -200; y < 300; y += mesh_size){
            for(var z = 0; z < 300; z += mesh_size){
                particles.push(new Particle(new Vector(-300, y, z), new Vector(1, 0, -0.5), GLOBL_LIFE, GLOBL_DEATH_RADIUS));
            }
        }
    }
    totalCollsisions = 0;
    run_sim = true;
}

function draw(){
  orbitControl();
  background(0);
  rotate(PI/3, [1, 0, 0]);

  if(run_sim){
      var newCasted = [];
      for(p of particles){
          if(p.collidePlaneMirror(mirror)){
              totalCollsisions++;
          }

          p.collideSphere(s1, newCasted);
          p.updatePos();
      }

      /* delete dead particles */
      for(var i = particles.length - 1; i >= 0; i--){
          if(particles[i].life <= 0){
                particles.splice(i, 1);
            }
      }

      /* add casted particles */
      for(p of newCasted){
          particles.push(p);
      }
    }

    for(p of particles){
        p.show();
    }
    mirror.show();
    s1.show();

    document.getElementById('no_collisions').innerHTML = "Total collisions with the mirror: " + totalCollsisions;
    document.getElementById('no_rays').innerHTML = "Number of rays: " + particles.length;
}
