$fa=1;$fs=1;
module danmen(){
	intersection(){
		difference(){
			square([15/2,3.5]);
			translate([0,15+2.5])circle(15);
			translate([15/2-2,0])rotate(30)translate([0,-5])square([5,5]);
		}
		translate([0,-10])circle(15);
	}
}

difference(){
	rotate_extrude(){
		danmen();
	}


	linear_extrude(5){
		union(){
			intersection(){
				circle(d=4);
				square([4,3],center=true);
			}
			
			intersection(){
				difference(){
					circle(d=7);
					circle(d=6);
				}
				square([7,5],center=true);
			}
			
			square([6.5,.5],center=true);
			square([.5,6.5],center=true);
		}
	}
}

