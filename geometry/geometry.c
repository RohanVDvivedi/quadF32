#include<geometry/geometry.h>

vector zero_vector = {0.0, 0.0, 0.0};

void sum(vector* C, vector* A, vector* B)
{
	C->xi = A->xi + B->xi;
	C->yj = A->yj + B->yj;
	C->zk = A->zk + B->zk;
}

void diff(vector* C, vector* A, vector* B)
{
	C->xi = A->xi - B->xi;
	C->yj = A->yj - B->yj;
	C->zk = A->zk - B->zk;
}

void multiply_scalar(vector* C, vector* A, double sc)
{
	C->xi = A->xi * sc;
	C->yj = A->yj * sc;
	C->zk = A->zk * sc;
}

void cross(vector* C, vector* A, vector* B)
{
	C->xi = (A->yj * B->zk) - (A->zk * B->yj);
	C->yj = (A->zk * B->xi) - (A->xi * B->zk);
	C->zk = (A->xi * B->yj) - (A->yj * B->xi);
}

double dot(vector* A, vector* B)
{
	return (A->xi * B->xi) + (A->yj * B->yj) + (A->zk * B->zk);
}

double angle_between_vectors(vector* A, vector* B)
{
	double cosine = dot(A, B)/(magnitude_vector(A)*magnitude_vector(B));
	if(cosine >= 1.0)
	{
		return 0;
	}
	else if(cosine <= -1.0)
	{
		return 180;
	}
	else
	{
		return (acos(cosine) * 180) / M_PI;
	}
}

int unit_vector(vector* unitResult, vector* A)
{
	int failed_finding_unit_vector = 0;
	double magnit = magnitude_vector(A);
	if(magnit == 0.0)
	{
		// small error hurts no man
		magnit = 0.000001;
		failed_finding_unit_vector = -1;
	}
	multiply_scalar(unitResult, A, 1.0/magnit);
	return failed_finding_unit_vector;
}

// C = component of A parallel to B
void parallel_component(vector* C, vector* A, vector* B)
{
	// this step makes C = unit vector in direction of B
	unit_vector(C, B);

	// this is the magnitude of the component of A in direction of B
	double parallel_component_magnitude = dot(A, B) / magnitude_vector(B);

	// multiply magnitude and the direction
	multiply_scalar(C, C, parallel_component_magnitude);
}

// C = component of A perpendicular to B
void perpendicular_component(vector* C, vector* A, vector* B)
{
	// here we made c parallel componnet of A
	parallel_component(C, A, B);

	// C = A - component of A parallel to B = component of A perpendiculat to B
	diff(C, A, C);
}

double magnitude_vector(vector* A)
{
	vector temp = *A;
	return sqrt(dot(A, &temp));
}

double norm(quaternion* A)
{
	return sqrt((A->sc * A->sc) + (A->xi * A->xi) + (A->yj * A->yj) + (A->zk * A->zk));
}

void hamilton_product(quaternion* C, quaternion* A, quaternion* B)
{
	C->sc = (A->sc * B->sc) - (A->xi * B->xi) - (A->yj * B->yj) - (A->zk * B->zk);
	C->xi = (A->sc * B->xi) + (A->xi * B->sc) + (A->yj * B->zk) - (A->zk * B->yj);
	C->yj = (A->sc * B->yj) - (A->xi * B->zk) + (A->yj * B->sc) + (A->zk * B->xi);
	C->zk = (A->sc * B->zk) + (A->xi * B->yj) - (A->yj * B->xi) + (A->zk * B->sc);
}

void to_quaternion(quaternion* destination, quat_raw* source)
{
	double theta_in_radians = (source->theta) * M_PI / 180.0;
	double sine   = sin(theta_in_radians / 2);
	double cosine = cos(theta_in_radians / 2);
	destination->sc = cosine;
	destination->xi = sine * source->vectr.xi;
	destination->yj = sine * source->vectr.yj;
	destination->zk = sine * source->vectr.zk;
}

void get_unit_rotation_axis(vector* axis, quaternion* source)
{
	axis->xi = source->xi;
	axis->yj = source->yj;
	axis->zk = source->zk;
	unit_vector(axis, axis);
}

void conjugate(quaternion* destination, quaternion* source)
{
	destination->sc = +(source->sc);
	destination->xi = -(source->xi);
	destination->yj = -(source->yj);
	destination->zk = -(source->zk);
}

void reciprocal(quaternion* destination, quaternion* source)
{
	// take conjugate
	conjugate(destination, source);

	// divide by the square of norm
	double norm_1 	= norm(source);
	double norm_2 	= norm_1 * norm_1;

	destination->sc = destination->sc / norm_2;
	destination->xi = destination->xi / norm_2;
	destination->yj = destination->yj / norm_2;
	destination->zk = destination->zk / norm_2;
}

// F is the final vector we get by rotating I by a quaternion R
// F = R * I * Rconj
double rotate_vector(vector* F, quaternion* R, vector* I)
{
	quaternion Rinverse;
	reciprocal(&Rinverse, R);

	quaternion Itemp;
	Itemp.sc = 0;
	Itemp.xi = I->xi;
	Itemp.yj = I->yj;
	Itemp.zk = I->zk;

	quaternion temp;

	quaternion Ftemp;

	hamilton_product(&temp, 	R, &Itemp );
	hamilton_product(&Ftemp, 	&temp, 		&Rinverse	);

	F->xi = Ftemp.xi;
	F->yj = Ftemp.yj;
	F->zk = Ftemp.zk;

	return Ftemp.sc;
}

void update_vector(vector* result, vector* new_value, double factor)
{
	result->xi = result->xi * (1 - factor) + new_value->xi * factor;
	result->yj = result->yj * (1 - factor) + new_value->yj * factor;
	result->zk = result->zk * (1 - factor) + new_value->zk * factor;
}

void slerp_quaternion(quaternion* Result, quaternion* A, double factorA, quaternion* B)
{
	if(factorA == 0.0)
	{
		(*Result) = (*B);
		return;
	}
	else if(factorA == 1.0)
	{
		(*Result) = (*A);
		return;
	}

	double dot = (A->sc * B->sc + A->xi * B->xi + A->yj * B->yj + A->zk * B->zk);
	dot = dot / ((A->sc * A->sc + A->xi * A->xi + A->yj * A->yj + A->zk * A->zk) * (B->sc * B->sc + B->xi * B->xi + B->yj * B->yj + B->zk * B->zk));
	
	quaternion B_ = (*B);
	quaternion A_ = (*A);
	
	if(dot < 0)
	{
		if(A_.sc > B_.sc)
		{
			B_.xi = -B_.xi;
			B_.yj = -B_.yj;
			B_.zk = -B_.zk;
			B_.sc = -B_.sc;
		}
		else
		{
			A_.xi = -A_.xi;
			A_.yj = -A_.yj;
			A_.zk = -A_.zk;
			A_.sc = -A_.sc;
		}
		dot = -dot;
	}

	double thet;
	if(dot >= 1)
	{
		thet = 0;
	}
	else
	{
		thet = acos(dot);
	}

	double sinet = sin(thet * factorA);
	double sinet_1 = sin(thet * (1 - factorA));
	double sine = sin(thet);

	if(sine <= 0.005)
	{
		Result->sc = (A_.sc * factorA + B_.sc * (1 - factorA));
		Result->xi = (A_.xi * factorA + B_.xi * (1 - factorA));
		Result->yj = (A_.yj * factorA + B_.yj * (1 - factorA));
		Result->zk = (A_.zk * factorA + B_.zk * (1 - factorA));
		double norm = (Result->sc * Result->sc + Result->xi * Result->xi + Result->yj * Result->yj + Result->zk * Result->zk);
		Result->sc = Result->sc/norm;
		Result->xi = Result->xi/norm;
		Result->yj = Result->yj/norm;
		Result->zk = Result->zk/norm;
		Result->sc = Result->sc > 1.0 ? 1.0 : Result->sc;
		Result->sc = Result->sc < -1.0 ? -1.0 : Result->sc;
	}
	else
	{
		Result->sc = (A_.sc * sinet + B_.sc * sinet_1)/sine;
		Result->xi = (A_.xi * sinet + B_.xi * sinet_1)/sine;
		Result->yj = (A_.yj * sinet + B_.yj * sinet_1)/sine;
		Result->zk = (A_.zk * sinet + B_.zk * sinet_1)/sine;
	}
}

void get_quaternion_from_vectors_changes(quaternion* quat, vector* Af, vector* Ai, vector* Bf, vector* Bi)
{
	vector A;diff(&A, Af, Ai);
	vector B;diff(&B, Bf, Bi);

	double perUnitMagDiffA = magnitude_vector(&A)/magnitude_vector(Ai);
	double perUnitMagDiffB = magnitude_vector(&B)/magnitude_vector(Bi);

	// this is the raw quaternion change from the sensors
	quat_raw raw;

	// if there is not change in magnitude of A, the rotation is probably in direction of Ai
	if(perUnitMagDiffA <= 0.02 && perUnitMagDiffB > 0.02)
	{
		// axis from A
		unit_vector(&(raw.vectr), Ai);
		// rotation angle from B
		vector Bip; vector Bfp;
		perpendicular_component(&Bip, Bi, &(raw.vectr));
		perpendicular_component(&Bfp, Bf, &(raw.vectr));
		raw.theta = angle_between_vectors(&Bfp, &Bip);
	}
	// if there is not change in magnitude of A, the rotation is probably in direction of Bi
	else if(perUnitMagDiffA > 0.02 && perUnitMagDiffB <= 0.02)
	{
		// axis from B
		unit_vector(&(raw.vectr), Bi);
		// rotation angle from A
		vector Aip; vector Afp;
		perpendicular_component(&Aip, Ai, &(raw.vectr));
		perpendicular_component(&Afp, Af, &(raw.vectr));
		raw.theta = angle_between_vectors(&Afp, &Aip);
	}
	// else we would have to fuse to figure out the axis of rotation
	else
	{
		double ybyz = 0.0;
		if( ( (A.zk * B.xi) - (A.xi * B.zk) ) != 0.0 )
		{
			ybyz = - ( ( (A.zk * B.xi) - (A.xi * B.zk) ) / ( (A.yj * B.xi) - (A.xi * B.yj) ) );
		}

		double xbyy = 0.0;
		if( ( (A.yj * B.zk) - (A.zk * B.yj) ) != 0.0 )
		{
			xbyy = - ( ( (A.yj * B.zk) - (A.zk * B.yj) ) / ( (A.xi * B.zk) - (A.zk * B.xi) ) );
		}

		double xbyz = 0.0;
		if( ( (A.zk * B.yj) - (A.yj * B.zk) ) != 0.0 )
		{
			xbyz = - ( ( (A.zk * B.yj) - (A.yj * B.zk) ) / ( (A.xi * B.yj) - (A.yj * B.xi) ) );
		}

		raw.vectr.zk = sqrt(1.0/(1.0 + (ybyz * ybyz) + (xbyz * xbyz)));
		if(ybyz == 0)
		{
			raw.vectr.yj = 0.0;
		}
		else
		{
			raw.vectr.yj = sqrt((ybyz * ybyz) / (1.0 + ((ybyz * ybyz) * (1.0 + (xbyy * xbyy)))));
		}
		if(xbyy == 0 || xbyz == 0)
		{
			raw.vectr.xi = 0.0;
		}
		else
		{
			raw.vectr.xi = sqrt((xbyy * xbyy * xbyz * xbyz)/((xbyy * xbyy) + (xbyz * xbyz) + (xbyy * xbyy * xbyz * xbyz)));
		}

		if(xbyy < 0)
		{
			raw.vectr.yj = -raw.vectr.yj;
		}
		if(xbyz < 0)
		{
			raw.vectr.zk = -raw.vectr.zk;
		}

		// find vector components perpendicuilar to raw.vectr
		vector Aip; vector Afp;
		perpendicular_component(&Aip, Ai, &(raw.vectr));
		perpendicular_component(&Afp, Af, &(raw.vectr));
		vector Bip; vector Bfp;
		perpendicular_component(&Bip, Bi, &(raw.vectr));
		perpendicular_component(&Bfp, Bf, &(raw.vectr));

		// this is the vecotor in same or opposite direction of raw.vectr
		vector AipCrossAfp;
		cross(&AipCrossAfp, &Aip, &Afp);
		double angle_AipCrossAfp_raw = angle_between_vectors(&AipCrossAfp, &(raw.vectr));
		double raw_vectr_sign_inversion_required_a = angle_AipCrossAfp_raw > 170 ? -1 : 1;

		// this is the vecotor in same or opposite direction of raw.vectr
		vector BipCrossBfp;
		cross(&BipCrossBfp, &Bip, &Bfp);
		double angle_BipCrossBfp_raw = angle_between_vectors(&BipCrossBfp, &(raw.vectr));
		double raw_vectr_sign_inversion_required_b = angle_BipCrossBfp_raw > 170 ? -1 : 1;

		// find per unit change perpendicular change difference
		// higher this value higher is the accuracy of the result angle from that vector sensor
		vector Aperp_mag_diff; diff(&Aperp_mag_diff, &Afp, &Aip);
		vector Bperp_mag_diff; diff(&Bperp_mag_diff, &Bfp, &Bip);
		double factorA = magnitude_vector(&Aperp_mag_diff)/magnitude_vector(Ai);
		double factorB = magnitude_vector(&Bperp_mag_diff)/magnitude_vector(Bi);

		// use the factors
		if(factorA > factorB)
		{
			multiply_scalar(&(raw.vectr), &(raw.vectr), raw_vectr_sign_inversion_required_a);
		}
		else
		{
			multiply_scalar(&(raw.vectr), &(raw.vectr), raw_vectr_sign_inversion_required_b);
		}

		// exponentially scale them, to find probalistically the single better angle
		factorA = pow(2.718, 1 + factorA*3.0);
		factorB = pow(2.718, 1 + factorB*3.0);
		factorA = factorA / (factorA + factorB);
		factorB = 1 - factorA;

		double angle_by_A = angle_between_vectors(&Afp, &Aip);
		double angle_by_B = angle_between_vectors(&Bfp, &Bip);

		raw.theta = factorA * angle_by_A + factorB * angle_by_B;
	}

	to_quaternion(quat, &raw);
}