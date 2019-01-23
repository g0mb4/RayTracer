const EPSILON = 0.00001;

class Particle {
    constructor(p, v, e, d){
        this.pos = p;
        this.vel = v;
        this.energy = e;
        this.death_radius = d;

        this.path = [];

        this.spawn_lock = 0;
        this.spawn_lock_max = 30;

        /* two points of the line */
        this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
        this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
    }

    // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
    collidePlaneMirror(m){
        if(this.pos && this.vel && this.pos.z + this.vel.z <= 0){
            this.vel = this.vel.subtract(m.normal.multiply(2 * this.vel.dot(m.normal)));

            /* create new points for the next line */
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            return true;
        } else {
            return false;
        }
    }

    _solveQuadratic(a, b, c) {
        var result1 = (-1 * b + Math.sqrt(Math.pow(b, 2) - (4 * a * c))) / (2 * a);
        var result2 = (-1 * b - Math.sqrt(Math.pow(b, 2) - (4 * a * c))) / (2 * a);
        return {x1: result1, x2: result2 }
    }

    _polarizedReflection(n1, n2, cos_a1, cos_a2){
      var left  = n1 * cos_a1;
      var right = n2 * cos_a2;
      var num = left - right;
      var den = left + right;

      var den2 = den * den;
      if(den2 < EPSILON){
          return 1; // Assume complete reflection.
      }

      var abs = Math.abs((num * num) / (den * den));
      var reflection = abs * abs;
      if (reflection > 1.0){
          // Clamp to actual upper limit.
          return 1.0;
      }
      return reflection;
    }

    // https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
    // http://www.starkeffects.com/snells-law-vector.shtml
    // http://cosinekitty.com/raytrace/chapter09_refraction.html
    collideSphere(s, casted){
        var d_n1 = Vector.distance(this.pos, s.pos);
        var d_n2 = Vector.distance(this.pos.add(this.vel), s.pos);

        var collision = false;  // no collision
        var pos = null;
        var normal = null;
        var n1 = 1;
        var n2 = 1;

        // entered to the sphere
        if(d_n1 > s.r && d_n2 <= s.r){
          collision = true;
          pos = this.pos;
          normal = s.pos.subtract(pos).unit();
          n1 = 1;   // source: air
          n2 = s.n; // source: sphere

        // exited from the sphere
        } else if(d_n1 < s.r && d_n2 >= s.r){
          collision = true;
          pos = this.pos.add(this.vel);
          normal = s.pos.subtract(pos).unit();
          n1 = s.n;   // source: air
          n2 = 1;     // source: sphere
        }

        if(collision){
          var pos = this.pos;
          var normal = s.pos.subtract(pos).unit();

          // reflected
          var vel_reflected = this.vel.subtract(normal.multiply(2 * this.vel.dot(normal))).unit();
          //console.log("enter_reflect");

          // refracted
          var cos_a1 = this.vel.dot(normal);
          var sin_a1 = 0;

          if(cos_a1 <= -1.0){
              cos_a1 = -1.0;  // clamp to lower limit
              sin_a1 =  0.0;
          } else if(cos_a1 >= +1.0){
              cos_a1 = +1.0;  // clamp to upper limit
              sin_a1 =  0.0;
          } else {
              sin_a1 = sqrt(1.0 - cos_a1 * cos_a1);
          }

          var n_ratio = n1 / n2;
          var sin_a2 = n_ratio * sin_a1;
          var E_refracted = 0;
          var E_reflected = this.energy;

          if(sin_a2 <= -1.0 || sin_a2 >= +1.0){
              // Since sin_a2 is outside the bounds -1..+1, then
              // there is no such real angle a2, which in turn
              // means that the ray experiences total internal reflection,
              // so that no refracted ray exists.
          } else {
              var res = this._solveQuadratic(1.0, 2.0 * cos_a1, 1.0 - 1.0/(n_ratio*n_ratio));
              var max_alignment = -0.0001;
              var refract_dir = new Vector(0, 0, 0);
              if(!isNaN(res.x1)){
                  var refract_attempt = this.vel.add(normal.multiply(res.x1));
                  var alignment = this.vel.dot(refract_attempt);
                  if (alignment > max_alignment){
                      max_alignment = alignment;
                      refract_dir = refract_attempt;
                  }
              }

              if(!isNaN(res.x2)){
                  var refract_attempt = this.vel.add(normal.multiply(res.x2));
                  var alignment = this.vel.dot(refract_attempt);
                  if (alignment > max_alignment){
                      max_alignment = alignment;
                      refract_dir = refract_attempt;
                  }
              }

              if (max_alignment <= 0.0){
                  // Getting here means there is something wrong with the math.
                  // Either there were no solutions to the quadratic equation,
                  // or all solutions caused the refracted ray to bend 90 degrees
                  // or more, which is not possible.
                  E_reflected = this.energy;
                  E_refracted = 0;
             } else {
                  // https://en.wikipedia.org/wiki/Fresnel_equations

                  var cos_a2 = Math.sqrt(1.0 - sin_a2*sin_a2);
                  if (cos_a1 < 0.0){
                      // Tricky bit: the polarity of cos_a2 must
                      // match that of cos_a1.
                      cos_a2 = -cos_a2;
                  }

                  // We assume uniform polarization of light,
                  // and therefore average the contributions of s-polarized
                  // and p-polarized light.
                  var R_s = this._polarizedReflection(n1, n2, cos_a1, cos_a2);
                  var R_p = this._polarizedReflection(n1, n2, cos_a2, cos_a1);
                  var R_eff = (R_s + R_p) / 2.0;

                  E_refracted = (1.0 - R_eff) * this.energy;
                  E_reflected = R_eff * this.energy;
             }
          }

          this.spawn_lock = this.spawn_lock_max;   // solve double cast (numeric error)
          casted.push(new Particle(pos, vel_reflected, E_reflected, this.death_radius));

          this.energy = E_refracted;
          this.vel = refract_dir;

          /* create new points for the next line */
          this.path.push({x: pos.x, y: pos.y, z: pos.z});
          this.path.push({x: pos.x, y: pos.y, z: pos.z});

        }
    }

    updatePos(){
        this.pos = this.pos.add(this.vel);

        /* just update teh 2nd position of the line */
        if(this.death_radius > 0 && Vector.distance(this.pos, new Vector(0, 0, 0)) >= this.death_radius){
            this.energy = 0;
        } else {
            var len = this.path.length;
            this.path[len - 1].x = this.pos.x;
            this.path[len - 1].y = this.pos.y;
            this.path[len - 1].z = this.pos.z;
        }
    }

    show(){
        noFill();
        stroke(255, 255, 0);

        beginShape(LINES);
        for(var i = 0; i < this.path.length; i += 2){
            vertex(this.path[i    ].x, this.path[i    ].y, this.path[i    ].z);
            vertex(this.path[i + 1].x, this.path[i + 1].y, this.path[i + 1].z);
        }
        endShape();
    }
}

class Sphere {
    constructor(p, r, n){
        this.pos = p;
        this.r = r;
        this.n = n;
    }

    show(){
        push();
            noFill();
            stroke(0, 0, 255);
            translate(this.pos.x, this.pos.y, this.pos.z);
            sphere(this.r);
        pop();
    }
}

class Plane {
    constructor(w, h, n){
        this.w = w;
        this.h = h;
        this.normal = n.unit();
    }

    show(){
        push();
            noFill();
            stroke(0, 255, 0);
            plane(this.w, this.h);
        pop();
    }
}
