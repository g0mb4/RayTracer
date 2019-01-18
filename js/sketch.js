var s1;
var mirror;
var particles = [];

var PCollMirror;
var PRays;

var totalCollsisions;

function setup() {
    createCanvas(800, 600, WEBGL);

    mirror = new Plane(1000, 1000, new Vector(0, 0, 1));
    s1 = new Sphere(new Vector(0, 0, 100), 100, 4/3);

    particles.push(new Particle(new Vector(-300, 0, 100), new Vector(1, 0, -0.5)));

    totalCollsisions = 0;
    PCollMirror = createP();
    PRays = createP();
}

function draw(){
  background(0);
  orbitControl();

  rotate(PI/3, [1, 0, 0]);

  var newCasted = [];
  for(p of particles){
      if(p.collidePlaneMirror(mirror)){
          totalCollsisions++;
      }
      
      p.collideSphere(s1, newCasted);
      p.updatePos();
      p.show()
  }
  for(p of newCasted){
      particles.push(p);
  }

  mirror.show();
  s1.show();

  PCollMirror.html("Total collisions with the mirror: " + totalCollsisions);
  PRays.html("Number of rays: " + particles.length);
}
