var s1;
var mirror;
var particles = [];

var totalCollsisions;
var run_sim;

function setup() {
    var canvas = createCanvas(800, 600, WEBGL);
    canvas.parent('canvas');

    mirror = new Plane(1000, 1000, new Vector(0, 0, 1));
    s1 = new Sphere(new Vector(0, 0, 100), 100, 4/3);

    simRestart();
    run_sim = false;
}

function simRestart(){
    var life = parseInt(document.getElementById('life').value);
    var death_radius = parseInt(document.getElementById('death_radius').value);

    var width_of_lamp = parseInt(document.getElementById('width_of_lamp').value);
    var height_of_lamp = parseInt(document.getElementById('height_of_lamp').value);

    var no_beams = parseInt(document.getElementById('number_of_beams').value);

    var mesh_size_y = Math.floor(width_of_lamp/Math.sqrt(no_beams));
    var mesh_size_z = Math.floor(height_of_lamp/Math.sqrt(no_beams));


    var cos_alpha = Math.cos(radians(parseFloat(document.getElementById('alpha').value)));
    var cos_beta = Math.cos(radians(parseFloat(document.getElementById('beta').value)));
    var cos_gamma = Math.cos(radians(parseFloat(document.getElementById('gamma').value)));

    var v0 = new Vector(cos_alpha, cos_beta, cos_gamma);

    particles = [];
    var n = 0;
    if(no_beams < 0){
        particles.push(new Particle(new Vector(-300, 0, 100), v0, life, death_radius));
    } else {
        for(var y = -width_of_lamp/2; y < width_of_lamp/2; y += mesh_size_y){
            for(var z = 1; z < height_of_lamp; z += mesh_size_z){
                particles.push(new Particle(new Vector(-300, y, z), v0, life, death_radius));
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

    document.getElementById('infos').innerHTML = "total collisions with the mirror: " + totalCollsisions + " , number of rays: " + particles.length;
}
