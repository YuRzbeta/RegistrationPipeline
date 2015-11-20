#include "pipeline.h"


/* CONSTRUCTOR ------------------------------------------------------------
 *
 * Creates the pipline class. Creates Data object and sets a pointer
 * to data from all steps of the pipeline.
 */
Pipeline::Pipeline(){

}


/* CONSTRUCTOR ------------------------------------------------------------
 *
 * Creates the pipline class. Creates Data object and sets a pointer
 * to data from all steps of the pipeline. Parameters are provided by
 * a file.
 */
Pipeline::Pipeline(char * paramsfile){

    detection = NULL;
    description = NULL;
    searching = NULL;
    refinement = NULL;

    data = new Data(paramsfile);

    createMethods();
}


/* DESTRUCTOR ------------------------------------------------------------
 *
 */
Pipeline::~Pipeline(){

    delete data;

    if(detection!=NULL) delete detection;
    if(description!=NULL) delete description;
    if(searching!=NULL) delete searching;
    if(refinement!=NULL) delete refinement;
}

void Pipeline::createMethods(){

    // Input Handler ---

    // Detection ---
    if(data->params.useDetection){
        if(data->params.detectMethod == "ISS"){
            detection = new det_ISS();
            detection->setData(data);
        }
        else if(data->params.detectMethod == "RandomSampling"){
            detection = new det_RandomSampling();
            detection->setData(data);
        }
        else if(data->params.detectMethod == "NormalSpaceSampling"){
            detection = new det_NormalSpaceSampling();
            detection->setData(data);
        }
        else if(data->params.detectMethod == "ColorSpaceSampling"){
            detection = new det_ColorSpaceSampling();
            detection->setData(data);
        }
        else{
            cerr << "ERROR: The detection method is not valid." << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Description ---
    if(data->params.useDescription){
        if(data->params.descMethod == "SHOT"){
            description = new des_SHOT();
            description->setData(data);

        }
        else if(data->params.descMethod == "SpinImage") {
            description = new des_SP();
            description->setData(data);
        }
        else{
            cerr << "ERROR: The description method is not valid." << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Searching Strategies ---
    if(data->params.useSS){
        if(data->params.SSMethod == "SmartForce"){
            searching = new ss_SmartForce();
            searching->setData(data);
        }
        else if(data->params.SSMethod == "4PCS"){
            searching = new ss_4PCS();
            searching->setData(data);
        }
        else if(data->params.SSMethod == "3PS"){
            searching = new ss_3PS();
            searching->setData(data);
        }
        else if(data->params.SSMethod == "Grid3D"){
            searching = new ss_Grid3D();
            searching->setData(data);
        }
        else{
            cerr << "ERROR: The Searching Strategy method is not valid." << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Refinement ---
    if(data->params.useRefinement){
        if(data->params.refineMethod == "ICP"){
            refinement = new ref_ICP();
            refinement->setData(data);

        }
        else{
            cerr << "ERROR: The refinement method is not valid." << endl;
            exit(EXIT_FAILURE);
        }
    }
}


/*  EXECUTE --------------------------------------------------------------
 *
 *  Execution of the pipeline. Each step has his own execute()
 *  method that implements the functionalities of each step.
 */
void Pipeline::execute(){

    cout << endl;
    cout << "3D REGISTRATION PIPELINE" << endl;
    cout << "----------------------------------------------" << endl;

    data->printParams();

    // Compute groundtruth
    if(!data->params.realData){

        calcGroundTruth(data->A, description);
    }

    Timer timer;

        input.setData(data);
        cout << "------------------------------------------------------------------------------------> INPUT START" << endl;
        input.execute();
        cout << endl;
        cout << "------------------------------------------------------------------------------------> INPUT DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;

    if(data->params.useDetection){

        cout << "------------------------------------------------------------------------------------> DETECTION START" << endl;
        detection->execute();
        cout << endl;
        cout << "------------------------------------------------------------------------------------> DETECTION DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;
    }

    if(data->params.useDescription){

        cout << "------------------------------------------------------------------------------------> DESCRIPTION START" << endl;
        description->execute();
        cout << endl;
        cout << "------------------------------------------------------------------------------------> DESCRIPTION DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;
    }

    if(data->params.useSS){

        cout << "------------------------------------------------------------------------------------> SEARCHING STRATEGIES START" << endl;
        searching->execute();
        applyMovement(COARSE);
        cout << endl;
        cout << "------------------------------------------------------------------------------------> SEARCHING STRATEGIES DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;
    }

    cout << "Coarse Alignment results:" << endl;
    computeResidue();

    if(data->params.useRefinement){

        cout << "------------------------------------------------------------------------------------> REFINEMENT START" << endl;
        refinement->execute();
        applyMovement(FINE);
        cout << endl;
        cout << "------------------------------------------------------------------------------------> REFINEMENT DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;
    }

    cout << "Fine Alignment results:" << endl;
    computeResidue();

        output.setData(data);
        cout << "------------------------------------------------------------------------------------> OUTPUT START" << endl;
        output.execute();
        cout << endl;
        cout << "------------------------------------------------------------------------------------> OUTPUT DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;

    timer.stop();

    cout << endl << "--------------------------------------------------------------------------------> REGISTRATION PIPELINE FINISHED" << endl << endl;

}


/*  EXECUTE TEST -------------------------------------------------------------------------------------
 *
 *  Execution of the pipeline for test execution. There are no information
 *  provided by the screen, only the results.
 */
void Pipeline::executeTest(){

    Timer timer;

    input.execute();

    cout << endl << endl;
    cout << "-----------------------------------------------------------------------------------------" << endl;
    cout << "Method:        " << data->params.descMethod << endl;
    cout << "Real Data:     " << data->params.realData << endl;
    cout << "Model:         " << data->params.infile << endl;
    cout << "Noise:         " << data->params.percOfNoise << endl;

    cout << "DESCTIME; SSPP; SSRES; SSTIME; ICPPP; ICPRES; ICPTIME;" << endl;

    if(data->params.useDetection){
        //cout << "--------- DETECTION --------" << endl;
        timer.reset();
        detection->execute();
        //cout << "Time: " << timer.elapsed() << endl;
        cout << timer.elapsed() << ";";

    }

    if(data->params.useDescription){
        //cout << "--------- DESCRIPTION --------" << endl;
        timer.reset();
        description->execute();
        //cout << "Time: " << timer.elapsed() << endl;
        cout << timer.elapsed() << ";";
    }

    if(data->params.useSS){
        //cout << "--------- SEARCHING STRATEGIES --------" << endl;
        timer.reset();
        searching->execute();
        //cout << "Time: " << timer.elapsed() << endl;
        cout << timer.elapsed() << ";";
    }

    if(data->params.useRefinement){
        //cout << "--------- REFINEMENT --------" << endl;
        timer.reset();
        refinement->execute();
        //cout << "Time: " << timer.elapsed() << endl;
        cout << timer.elapsed() << ";" << endl;

    }


    output.execute();

    timer.stop();

    cout << endl << endl;
}

void Pipeline::computeResidue(){

    int pairedPoints = 0;
    double res = data->A->calcNN(data->B->getPoints(), data->params.percOfPoints, data->params.nnErrorFactor, pairedPoints);

    int pairedPoints2 = 0;
    double res2 = data->B->calcNN(data->A->getPoints(), data->params.percOfPoints, data->params.nnErrorFactor, pairedPoints2);


    // Printing the results of the execution.
    cout << "% of paired points of A : " << ((float)pairedPoints / (float)data->A->allpoints->size() ) * 100 << "%" << endl;
    cout << "% of paired points of B: " << ((float)pairedPoints2 / (float)data->B->allpoints->size() ) * 100 << "%" << endl;
    cout << "Residue: " << res << endl;
}


/* CALC DESCRIPTOR GROUND TRUTH ---------------------------------------
*
*  This method calculates the descriptor-distance ground truth of a given point cloud.
*  We take a ElementSet and copy it. We perturb a little bit and we calculate
*  the difference between its descriptors. This is the ground truth for the tests.
*/
void Pipeline::calcGroundTruth(ElementSet *X, IDescription *desc) {

    // Create a copy of owr object.
    ElementSet aux(*X);

    // Add a little bit of noise.
    aux.addNoise(data->params.percOfNoise);

    // Add a little rotation.
    motion3D *m3 = new motion3D(0.001,1);
    aux.transform(m3);
    aux.createFileFromData(data->params.infileTemp);
    delete m3;


    if (data->params.useDescription){

        description->calcDescriptors(X);
        description->calcDescriptors(&aux);


        vector<Point>::iterator it2 = aux.getPoints()->begin();

        float dists = 0;
        float minDist = FLT_MAX;
        float maxDist = 0;
        int count = 0;

        for (vector<Point>::iterator it = X->getPoints()->begin(); it!= X->getPoints()->end(); ++it){

            float d = it->getDescriptor()->compare(it2->getDescriptor());

            if (!isnan(d)){

                if(d < minDist) minDist = d;
                if(d > maxDist) maxDist = d;

                dists = dists + d;
                count ++;
            }
            else{
                cerr << "WARNING: There is a NAN value in the descriptors!" << endl;
            }
            ++it2;
        }

        data->params.GTminDescDist = minDist;
        data->params.GTmaxDescDist = maxDist;
        data->params.GTdescThrs = dists/count;
    }

    //Calc a ground truth residue and % of paired points
    int pairedPoints = 0;
    data->params.GTresidue = X->calcNN(aux.getPoints(), data->params.percOfPoints, data->params.nnErrorFactor, pairedPoints);
    data->params.GTpercPairedPoints = (float)pairedPoints / (float) X->nPoints();
}

void Pipeline::applyMovement(int type) {

    if(type == COARSE){

        if(data->cM != NULL) {
            data->B->transform(data->cM);
            data->B->createFileFromData(data->params.infileTemp, false, true);
        }
        else{
            cerr << "ApplyMovement :: Coarse Movement Matrix is NULL!" << endl;
            exit(EXIT_FAILURE);
        }
    }
    else if(type == FINE){

        if(data->fM != NULL) {
            data->B->transform(data->fM);
        }
        else{
            cerr << "ApplyMovement :: Fine Movement Matrix is NULL!" << endl;
            exit(EXIT_FAILURE);
        }
    }
    else{
        cerr << "Pipeline :: No movement type specified" << endl;
    }

}
