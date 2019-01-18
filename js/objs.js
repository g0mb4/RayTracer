class Particle {
    constructor(p, v){
        this.pos = p;
        this.vel = v.unit();

        this.path = [];
        this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
        this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
    }

    // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
    collidePlaneMirror(m){
        if(this.pos.z + this.vel.z <= 0){
            this.vel = this.vel.subtract(m.normal.multiply(2 * this.vel.dot(m.normal)));

            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            return true;
        } else {
            return false;
        }
    }

    // https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
    collideSphere(s, casted){
        var d_n1 = Vector.distance(this.pos, s.pos);
        var d_n2 = Vector.distance(this.pos.add(this.vel), s.pos);

        // entered to the sphere
        if(d_n1 > s.r && d_n2 <= s.r){
            var pos = this.pos;
            var normal = s.pos.subtract(pos).unit();

            // reflected
            var vel_reflected = this.vel.subtract(normal.multiply(2 * this.vel.dot(normal)));
            casted.push(new Particle(pos, vel_reflected));
/*
            // refracted
            var n1 = 1; // air
            var n2 = s.k;

            var r = (n1 / n2);
            var c = normal.dot(this.vel);

            this.vel = this.vel.multiply(r).add(normal.multiply(r * c - Math.sqrt(1-(r*r)*(1-c*c))));
*/
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});

        // exited from the sphere
        } else if(d_n1 <= s.r && d_n2 > s.r){
            var pos = this.pos.add(this.vel);
            var normal = s.pos.subtract(pos).unit();

            // reflected
            var vel_reflected = this.vel.subtract(normal.multiply(2 * this.vel.dot(normal)));
            casted.push(new Particle(pos, vel_reflected));

            // refracted

            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
            this.path.push({x: this.pos.x, y: this.pos.y, z: this.pos.z});
        }
    }

    updatePos(){
        this.pos = this.pos.add(this.vel);

        if(this.pos.x < 300 && this.pos.x > -300){
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
