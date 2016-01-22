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
        else if(data->params.detectMethod == "KinectSuperSampling"){
            detection = new det_KinectSuperSampling();
            detection->setData(data);
        }
        else if(data->params.detectMethod == "DistConnectComponents"){
            detection = new det_DistConnectedComponents();
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
    computeResidue(false);

    if(data->params.useRefinement){

        cout << "------------------------------------------------------------------------------------> REFINEMENT START" << endl;
        refinement->execute();
        applyMovement(FINE);
        cout << endl;
        cout << "------------------------------------------------------------------------------------> REFINEMENT DONE in: " << timer.elapsed() << " sec." << endl; timer.reset();
        cout << endl << endl;
    }

    cout << "Fine Alignment results:" << endl;
    computeResidue(false);

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

    input.setData(data);
    input.execute();

//    cout << "VIEW_A; VIEW_B; #_A; #_B;";
//    cout << "DET_METHOD; DET_TIME; DET_#_A; DET_#_B;";
//    cout << "DESC_METHOD; DESC_TIME;";
//    cout << "SS_METHOD; SSTIME; SS_RES; SS_OVLP_A; SS_OVLP_B;";
//    cout << "REF_METHOD; REF_TIME; REF_RES; REF_OVLP_A; REF_OVLP_B;";
//    cout << endl;

    cout << data->params.infile << ";";
    cout << data->params.infile2 << ";";
    cout << data->A->nPoints() << ";";
    cout << data->B->nPoints() << ";";

    if(data->params.useDetection){
        cout << data->params.detectMethod << ";";
        timer.reset();
        detection->execute();
        cout << timer.elapsed() << ";";
        cout << data->A->nPoints() << ";";
        cout << data->B->nPoints() << ";";
    }
    else cout << "-;-;-;-;";

    if(data->params.useDescription){
        cout << data->params.descMethod << ";";
        timer.reset();
        description->execute();
        cout << timer.elapsed() << ";";
    }
    else cout << "-;-;";

    if(data->params.useSS){
        cout << data->params.SSMethod << ";";
        timer.reset();
        searching->execute();
        cout << timer.elapsed() << ";";
        applyMovement(COARSE);
        computeResidue(true);
    }
    else cout << "-;-;-;-;";

    if(data->params.useRefinement){
        cout << data->params.refineMethod << ";";
        timer.reset();
        refinement->execute();
        cout << timer.elapsed() << ";";
        applyMovement(FINE);
        computeResidue(true);
    }
    else cout << "-;-;-;-;";


    output.setData(data);
    output.execute();

    timer.stop();

    cout << endl;
}


void Pipeline::executeResidueComputation(){

    Timer timer;

    input.setData(data);
    input.execute();

    cout << endl << endl;
    cout << "-----------------------------------------------------------------------------------------" << endl;
    cout << "                                                   PIPELINE PROJECT - RESIDUE COMPUTATION" << endl;
    cout << "Target model:         " << data->params.infile << endl;
    cout << "Candidate model:      " << data->params.infile2 << endl;
    cout << "% of used points:     " << data->params.percOfPoints << endl << endl;

    timer.reset();
    computeResidue();
    cout << "Time: " << timer.elapsed() << " sec." << endl;
}


void Pipeline::computeResidue(bool test) {

    int pairedPoints = 0;
    double res = data->A->calcNN(data->B->getPoints(), data->params.percOfPoints, data->params.nnErrorFactor, pairedPoints);

    int pairedPoints2 = 0;
    double res2 = data->B->calcNN(data->A->getPoints(), data->params.percOfPoints, data->params.nnErrorFactor, pairedPoints2);


    if(test==false) {
        cout << "MMD A: " << data->A->getMMD() << " MMD B: " << data->B->getMMD() << endl;
        // Printing the results of the execution.
        cout << "% of paired points of A : " << ((float) pairedPoints / (float) data->A->allpoints->size()) * 100 <<
        "%" << endl;
        cout << "% of paired points of B: " << ((float) pairedPoints2 / (float) data->B->allpoints->size()) * 100 <<
        "%" << endl;
        cout << "Residue: " << res << endl;
    }
    else{
        cout << res << ";";
        cout << ((float) pairedPoints / (float) data->A->allpoints->size()) << ";";;
        cout << ((float) pairedPoints2 / (float) data->B->allpoints->size()) << ";";;
    }
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
