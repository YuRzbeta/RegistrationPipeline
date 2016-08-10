#datastruct[0]="kdtree"
#datastruct[1]="trihash"
#datastruct[2]="compressedOctree"
#datastruct[3]="S4PCSkdtree"
#datastruct[4]="gridtree"

datastruct[0]="gridtree"

for DS in "${datastruct[@]}"
do
  echo "<params>
      <files>
          <realData>true</realData>
          <infile>../../../../models/ResidueTests/bust0.ply</infile>
          <infile2>../../../../models/ResidueTests/bust1.ply</infile2>
          <infileTemp>./temp.ply</infileTemp>
          <outfile>./out.ply</outfile>
          <outres>/home/ferran/MEGA/PROJECTS/Pipeline/models/descriptors.txt</outres>
      </files>
      <methods>
          <detection use='false'>
              <method>ColorSpaceSampling</method>
              <properties>
                  <nSamples>1000</nSamples>
              </properties>
          </detection>

          <description use='false'>
              <method>SHOT</method>
              <properties>
                  <radiusNormalFactor>5</radiusNormalFactor>
                  <radiusSearchFactor>20</radiusSearchFactor>
                  <nNeighbours>100</nNeighbours>
              </properties>
          </description>

          <searchingStrategies use='true'>
              <!--<method>3PS</method>-->
              <method>4PCS</method>
              <!--<method>SmartForce</method>-->
              <properties>
                  <nCells>3</nCells>
                  <thrsFactor>1</thrsFactor>
              </properties>
          </searchingStrategies>

          <refinement use='true'>
              <method>ICP</method>
              <properties></properties>
          </refinement>

      </methods>

    <dataStructure>
        <name>$DS</name>
        <params>
            <param name='thrsKdtree' value='10' />
            <param name='slotSizeFactor' value='50'/> <!-- slotPerDim = diagonal/(MMD*slotSizeFactor) -->
        </params>
    </dataStructure>

      <generalProperties>
          <percOfPoints>1</percOfPoints>
          <nnErrorFactor>2</nnErrorFactor>
          <percOfNoise>0</percOfNoise> <!-- normalized % of MMD (perc * MMD) -->
          <normalizeModels>false</normalizeModels>
      </generalProperties>
  </params>" >> params.xml

  ../../Pipeline params.xml 1 matrixBust.xls
  rm -f params.xml
done
