var s1;
var mirror;
var particles = [];

var energy_limit;
var run_sim;

var total_collsisions;
var total_energy;
var minimal_system_energy = 0.00000001;

function setup() {
    var canvas = createCanvas(800, 600, WEBGL);
    canvas.parent('canvas');

    mirror = new Plane(1000, 1000, new Vector(0, 0, 1));
    s1 = new Sphere(new Vector(0, 0, 100), 100, 4/3);

    simRestart();
    run_sim = false;
}

function simRestart(){
    energy_limit = parseFloat(document.getElementById('energy_limit').value);
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
        particles.push(new Particle(new Vector(-300, 0, 100), v0, 1, death_radius));
    } else {
        for(var y = -width_of_lamp/2; y < width_of_lamp/2; y += mesh_size_y){
            for(var z = 1; z < height_of_lamp; z += mesh_size_z){
                particles.push(new Particle(new Vector(-300, y, z), v0, 1, death_radius));
            }
        }
    }
    total_collsisions = 0;
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
              total_collsisions++;
          }

          /* ugly way to solve numeric error :( */
          if(p.spawn_lock > 0){
              p.spawn_lock--;
          } else {
              p.collideSphere(s1, newCasted);
          }

          p.updatePos();
      }

      /* add casted particles */
      for(p of newCasted){
          particles.push(p);
      }

      /* delete dead particles */
      for(var i = particles.length - 1; i >= 0; i--){
          if(particles[i].energy < energy_limit){
                particles.splice(i, 1);
            }
      }
    }

    /* check energy */
    total_energy = 0;
    for(p of particles){
        total_energy += p.energy;
    }

    if(total_energy < minimal_system_energy){
      run_sim = false;
    }

    for(p of particles){
        p.show();
    }

    mirror.show();
    s1.show();

    var info_str = "";

    if(run_sim){
      info_str += "RUNNING";
    } else {
      if(total_energy < minimal_system_energy){
        info_str += "DONE";
      } else {
        info_str += "STOPPED";
      }
    }

    info_str += " , total collisions with the mirror: " + total_collsisions;
    info_str += " , number of rays: " + particles.length;
    info_str += ", energy of the system: " + total_energy;

    document.getElementById('infos').innerHTML = info_str;
}
