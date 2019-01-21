function solveQuadratic(a, b, c) {
    var result1 = (-1 * b + Math.sqrt(Math.pow(b, 2) - (4 * a * c))) / (2 * a);
    var result2 = (-1 * b - Math.sqrt(Math.pow(b, 2) - (4 * a * c))) / (2 * a);
    return {x1: result1, x2: result2 }
}

class Particle {
    constructor(p, v, l, d){
        this.pos = p;
        this.vel = v;
        this.life = l;
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
        if(this.pos.z + this.vel.z <= 0){
            this.vel = this.vel.subtract(m.normal.multiply(2 * this.vel.dot(m.normal)));

            /* create new points for the next line */
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            return true;
        } else {
            return false;
        }
    }

    // https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
    // http://www.starkeffects.com/snells-law-vector.shtml
    // http://cosinekitty.com/raytrace/chapter09_refraction.html
    collideSphere(s, casted){
        var d_n1 = Vector.distance(this.pos, s.pos);
        var d_n2 = Vector.distance(this.pos.add(this.vel), s.pos);
        // entered to the sphere
        if(d_n1 > s.r && d_n2 <= s.r){
            var pos = this.pos;
            var normal = s.pos.subtract(pos).unit();

            // reflected
            var vel_reflected = this.vel.subtract(normal.multiply(2 * this.vel.dot(normal))).unit();
            //console.log("enter_reflect");
            this.spawn_lock = this.spawn_lock_max;   // solve double cast (numeric error)
            casted.push(new Particle(pos, vel_reflected, this.life - 1, this.death_radius));

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

            var ratio = 1 / s.k;
            var sin_a2 = ratio * sin_a1;

            if(sin_a2 <= -1.0 || sin_a2 >= +1.0){
                // Since sin_a2 is outside the bounds -1..+1, then
                // there is no such real angle a2, which in turn
                // means that the ray experiences total internal reflection,
                // so that no refracted ray exists.
            } else {
                var res = solveQuadratic(1.0, 2.0 * cos_a1, 1.0 - 1.0/(ratio*ratio));
                var maxAlignment = -0.0001;
                var refractDir = new Vector(0, 0, 0);
                if(!isNaN(res.x1)){
                    var refractAttempt = this.vel.add(normal.multiply(res.x1));
                    var alignment = this.vel.dot(refractAttempt);
                    if (alignment > maxAlignment){
                        maxAlignment = alignment;
                        refractDir = refractAttempt;
                    }
                }

                if(!isNaN(res.x2)){
                    var refractAttempt = this.vel.add(normal.multiply(res.x2));
                    var alignment = this.vel.dot(refractAttempt);
                    if (alignment > maxAlignment){
                        maxAlignment = alignment;
                        refractDir = refractAttempt;
                    }
                }

                if (maxAlignment <= 0.0){
                    // Getting here means there is something wrong with the math.
                    // Either there were no solutions to the quadratic equation,
                    // or all solutions caused the refracted ray to bend 90 degrees
                    // or more, which is not possible.
                    this.vel = new Vector(0, 0, 0); // TODO: solve this
               } else {
                    this.vel = refractDir;
               }
            }

            /*
                TODO: https://en.wikipedia.org/wiki/Fresnel_equations
             */

            this.life--;
            /* create new points for the next line */
            this.path.push({x: pos.x, y: pos.y, z: pos.z});
            this.path.push({x: pos.x, y: pos.y, z: pos.z});

        // exited from the sphere
        } else if(d_n1 < s.r && d_n2 >= s.r){
            var pos = this.pos.add(this.vel);
            var normal = s.pos.subtract(pos).unit();

            // reflected
            var vel_reflected = this.vel.subtract(normal.multiply(2 * this.vel.dot(normal))).unit();
            //console.log("exit_reflect");
            this.spawn_lock = this.spawn_lock_max;  // solve double cast (numeric error)
            casted.push(new Particle(pos.subtract(this.vel), vel_reflected, this.life-1, this.death_radius));

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

            var ratio = s.k / 1;
            var sin_a2 = ratio * sin_a1;

            if(sin_a2 <= -1.0 || sin_a2 >= +1.0){
                // Since sin_a2 is outside the bounds -1..+1, then
                // there is no such real angle a2, which in turn
                // means that the ray experiences total internal reflection,
                // so that no refracted ray exists.
            } else {
                var res = solveQuadratic(1.0, 2.0 * cos_a1, 1.0 - 1.0/(ratio*ratio));
                var maxAlignment = -0.0001;
                var refractDir = new Vector(0, 0, 0);
                if(!isNaN(res.x1)){
                    var refractAttempt = this.vel.add(normal.multiply(res.x1));
                    var alignment = this.vel.dot(refractAttempt);
                    if (alignment > maxAlignment){
                        maxAlignment = alignment;
                        refractDir = refractAttempt;
                    }
                }

                if(!isNaN(res.x2)){
                    var refractAttempt = this.vel.add(normal.multiply(res.x2));
                    var alignment = this.vel.dot(refractAttempt);
                    if (alignment > maxAlignment){
                        maxAlignment = alignment;
                        refractDir = refractAttempt;
                    }
                }

                if (maxAlignment <= 0.0){
                    // Getting here means there is something wrong with the math.
                    // Either there were no solutions to the quadratic equation,
                    // or all solutions caused the refracted ray to bend 90 degrees
                    // or more, which is not possible.
                    this.vel = new Vector(0, 0, 0);
               } else {
                    this.vel = refractDir;
               }
            }

            this.life--;
            /* create new points for the next line */
            this.path.push({x: pos.x, y: pos.y, z: pos.z});
            this.path.push({x: pos.x, y: pos.y, z: pos.z});
        }
    }

    updatePos(){
        this.pos = this.pos.add(this.vel);

        /* just update teh 2nd position of the line */
        if(this.death_radius > 0 && Vector.distance(this.pos, new Vector(0, 0, 0)) >= this.death_radius){
            this.life = 0;
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
    constructor(p, r, k){
        this.pos = p;
        this.r = r;
        this.k = k;
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
