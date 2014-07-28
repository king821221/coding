package datafu.experiment.sampling;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

import org.apache.commons.math.random.RandomDataImpl;
import org.junit.Assert;

public class ProbabilityWeightedSampling {

    static class Sample {
        private static RandomDataImpl _RNG = new RandomDataImpl();

        int id;
        double weight;
        double randomFactor;
        double score;

        Sample(int id, double weight, double randomFactor) {
            this.id = id;
            this.weight = weight;
            this.score = 0;
            this.randomFactor = randomFactor;
        }

        Sample(int id, double weight) {
            this(id, weight, Math.random());
        }

        void resetRandomSeed(double randomFactor) {
            this.randomFactor = randomFactor;
        }

        double calculateScore(double p, double upperBound) {
            /*
             * weight <= upperBound -> weight/upperBound <= 1.0
             *        -> 1.0 + weight /upperBound <= 2.0
             *        -> 1.0 > 1.0 / (1.0 + weight / upperBound) >= 0.5
             */
            double fweight = 1.0 / (1.0 + weight / upperBound);
            this.score = this.randomFactor * fweight;
            return this.score;
        }

        double calculateBaselineScore(double upperBound) {
            this.score = 1.0 - Math.pow(this.randomFactor, 1.0 * upperBound / this.weight);
            return this.score;
        }

        @Override
        public int hashCode() {
            return this.id;
        }

        @Override
        public boolean equals(Object o) {
            if (o instanceof Sample) {
                Sample anotherSample = (Sample)o;
                return this.id == anotherSample.id;
            }

            return false;

        }
    }

    static class MapperOutput {
        String id;
        List<Sample> selectedSamples;
        List<Sample> acceptedSamples;
        int numItems;
        int numRejectedItems;
        double sumOfWeights;
        double upperBound;
        boolean useGlobalUpperBound;
        double q1;
        double q2;

        MapperOutput(String id,
                List<Sample> selectedSamples,
                List<Sample> acceptedSamples,
                int numItems,
                int numRejectedItems,
                double sumOfWeights,
                boolean useGlobalUpperBound,
                double upperBound,
                double q1,
                double q2) {
            this.id = id;
            this.selectedSamples = selectedSamples;
            this.acceptedSamples = acceptedSamples;
            this.numItems = numItems;
            this.numRejectedItems = numRejectedItems;
            this.sumOfWeights = sumOfWeights;
            this.useGlobalUpperBound = useGlobalUpperBound;
            this.upperBound = upperBound;
            this.q1 = q1;
            this.q2 = q2;
        }
    }

    static class CombinerOutput extends MapperOutput {
        List<MapperOutput> inputMappers = new ArrayList<MapperOutput>();
        int numRejectedInCombiner;
        CombinerOutput(String id,
                List<MapperOutput> inputMappers,
                List<Sample> selectedSamples,
                List<Sample> acceptedSamples,
                int numItems,
                int numRejectedItems,
                int numRejectedInCombiner,
                double sumOfWeights,
                boolean useGlobalUpperBound,
                double upperBound,
                double q1,
                double q2) {
            super(id, selectedSamples, acceptedSamples, numItems, numRejectedItems, sumOfWeights, useGlobalUpperBound, upperBound, q1, q2);
            this.inputMappers.addAll(inputMappers);
            this.numRejectedInCombiner = numRejectedInCombiner;
        }
    }

    static class ReducerOutput extends MapperOutput {
        List<CombinerOutput> inputCombiners = new ArrayList<CombinerOutput>();
        List<Sample> finalOutputSamples;
        int numRejectedInReducer;
        ReducerOutput(String id,
                List<CombinerOutput> inputCombiners,
                List<Sample> selectedSamples,
                List<Sample> acceptedSamples,
                List<Sample> finalOutputSamples,
                int numItems,
                int numRejectedItems,
                int numRejectedInReducer,
                double sumOfWeights,
                boolean useGlobalUpperBound,
                double upperBound,
                double q1,
                double q2) {
            super(id, selectedSamples, acceptedSamples, numItems, numRejectedItems, sumOfWeights, useGlobalUpperBound, upperBound, q1, q2);
            this.inputCombiners.addAll(inputCombiners);
            this.finalOutputSamples = finalOutputSamples;
            this.numRejectedInReducer = numRejectedInReducer;
        }
    }

    static class SampleWeightComparator implements Comparator<Sample> {
        @Override
        public int compare(Sample s1, Sample s2) {
            return Double.compare(s1.weight, s2.weight);
        }
    }

    static class SampleScoreComparator implements Comparator<Sample> {
        @Override
        public int compare(Sample s1, Sample s2) {
            return Double.compare(s1.score, s2.score);
        }
    }

    //q1 <= p + ({ - log(err) + sqrt(log(err) * log(err) - 2 * p * n * log(err)) } / {n + (sum(w(j), j = 1 to n) / upperBound) })
    private double calculateQ1(double sumOfWeights,
                               int n,
                               double upperBound,
                               double p,
                               double err) {
        double pn = p * n;

        double logerr = Math.log(err);

        double q1 = -logerr + Math.sqrt(logerr * logerr - 2 * pn * logerr);

        double reverseWeightSum = n + sumOfWeights/upperBound;
/*
        System.out.println("Calculate Q1 initial p: " + p  +
                ", n: " + n +
                ", upperBound: " + upperBound +
                ", logerr:" + logerr +
                ", q1: " + q1 +
                ", sumOfWeights: " + sumOfWeights +
                ", reverseWeightSum: " + reverseWeightSum);
*/
        return p + q1 / reverseWeightSum;
    }

    //q2 <= { p * n - 2 * log(err) / 3 - 2 * sqrt( log(err) * log(err) - 9 * p * n * log(err) / 2) / 3 } / sum(1 / f(w(j)), j = 1 to n)
    private double calculateQ2(double sumOfWeights,
                               int n,
                               double upperBound,
                               double p,
                               double err) {
        double pn = p * n;

        double logerr = Math.log(err);

        double q2 = pn - 2 *logerr / 3 - 2 * Math.sqrt(logerr * logerr - 9 * pn * logerr / 2) / 3;

        double reverseWeightSum = n + sumOfWeights / upperBound;

        /*
         System.out.println("Calculate Q2 initial p: " + p + ", n: " + n +
          ", upperBound: " + upperBound + ", logerr:" + logerr + ", q2: " + q2
          + ", sumOfWeights: " + sumOfWeights + ", reverseWeightSum: " +
          reverseWeightSum);
        */

        return q2 / reverseWeightSum;
    }

    //Simulate Mapper without global weight upper bound
    MapperOutput map(String mapperId,
            List<Sample> samples,
            double p,
            double err) {
        return map(mapperId, samples, p, err, false, 0);
    }

    //Simulate Mapper with gloal weight upper bound
    MapperOutput map(String mapperId,
            List<Sample> samples,
            double p,
            double err,
            boolean useGlobalUpperBound,
            double globalUpperBound) {
        List<Sample> selectedSamples = new ArrayList<Sample>(samples.size());

        List<Sample> acceptedSamples = new ArrayList<Sample>();

        double upperBound = (useGlobalUpperBound ? globalUpperBound : 0);

        double sumOfWeights = 0;

        for (Sample sample : samples) {
            if (!useGlobalUpperBound) {
                upperBound = Math.max(sample.weight, upperBound);
            } else if (Double.compare(sample.weight, globalUpperBound) > 0){
                throw new IllegalArgumentException("sample [" + sample.id
                        + "]'s weight [" + sample.weight
                        + "] is greater than global upper bound: " + globalUpperBound);
            }
            sumOfWeights += sample.weight;
        }

        double q1 = calculateQ1(sumOfWeights, samples.size(), upperBound, p, err);

        double q2 = calculateQ2(sumOfWeights, samples.size(), upperBound, p, err);

        int accepted = 0;
        int selected = 0;
        int rejected = 0;

        for (Sample sample: samples) {
            double score = sample.calculateScore(p, upperBound);
            if (useGlobalUpperBound) {
                if (score < q2) {
                    /*
                    System.out.println("mapper accept sample id: " + sample.id +
                            ", weight: " + sample.weight +
                            ", score: " + score);
                    */
                    accepted++;
                    acceptedSamples.add(sample);
                    continue;
                }
            }
            if (score < q1) {
                /*
                System.out.println("mapper selected sample id: " + sample.id +
                        ", weight: " + sample.weight +
                        ", score: " + score);
                */
                selected++;
                selectedSamples.add(sample);
            } else {
                rejected++;
            }
        }


        System.out.println("mapper[ + " + mapperId + "] selected: " + selected +
                ", rejected = " + rejected +
                ", accepted = " + accepted +
                ", n = " + samples.size() +
                ", p: " + p +
                ", q1: " + q1 +
                ", q2: " + q2 +
                ", sumOfWeights: " + sumOfWeights +
                ", upperBound: " + upperBound);

        return new MapperOutput(mapperId, selectedSamples, acceptedSamples, samples.size(), rejected, sumOfWeights, useGlobalUpperBound, upperBound, q1, q2);
    }

    //Simulate combiner
    CombinerOutput combine(String combinerId, List<MapperOutput> mapperOutputs, double p, double err) {
        int numItems = 0;
        int numRejectedItems = 0;
        double upperBound = 0;
        double sumOfWeights = 0;
        boolean useGlobalUpperBound = false;

        List<Sample> combinerSelectedSamples = new ArrayList<Sample>();

        List<Sample> aggMapperSelectedSamples = new ArrayList<Sample>();

        List<Sample> combinerAcceptedSamples = new ArrayList<Sample>();

        for (MapperOutput mapperOutput : mapperOutputs) {
            numItems += mapperOutput.numItems;
            sumOfWeights += mapperOutput.sumOfWeights;
            numRejectedItems += mapperOutput.numRejectedItems;
            upperBound = Math.max(upperBound, mapperOutput.upperBound);
            useGlobalUpperBound |= mapperOutput.useGlobalUpperBound;
            aggMapperSelectedSamples.addAll(mapperOutput.selectedSamples);
            combinerAcceptedSamples.addAll(mapperOutput.acceptedSamples);
        }

        double q1 = calculateQ1(sumOfWeights, numItems, upperBound, p, err);

        double q2 = calculateQ2(sumOfWeights, numItems, upperBound, p, err);

        for (MapperOutput mapperOutput : mapperOutputs) {
            if (Double.compare(q1, mapperOutput.q1) > 0) {
                throw new RuntimeException("q1 is increased in combiner: [" +
                          q1 + ", " + mapperOutput.q1 + "]");
            }
            if (Double.compare(q2, mapperOutput.q2) < 0) {
                throw new RuntimeException("q2 is decreased in combiner: [" +
                        q2 + ", " + mapperOutput.q2 + "]");
            }
        }

        int selected = 0;
        int accepted = 0;
        int rejected = 0;

        for (Sample sample: aggMapperSelectedSamples) {
            double score = sample.calculateScore(p, upperBound);
            if (useGlobalUpperBound) {
                if (score < q2) {
                    /*
                    System.out.println("combiner accept sample id: " + sample.id +
                            ", weight: " + sample.weight +
                            ", score: " + score);
                    */
                    accepted++;
                    combinerAcceptedSamples.add(sample);
                    continue;
                }
            }
            if (score < q1) {
                /*
                System.out.println("combiner selected sample id: " + sample.id +
                        ", weight: " + sample.weight +
                        ", score: " + sample.score);
                        */
                selected++;
                combinerSelectedSamples.add(sample);
            } else {
                rejected++;
            }
        }

        List<String> fromMappers = new ArrayList<String>();

        for(MapperOutput mapperOutput : mapperOutputs) {
            fromMappers.add(mapperOutput.id);
        }

        System.out.println("combiner[ + " + combinerId + "] selected: " + selected +
                ", rejected = " + rejected +
                ", accepted = " + accepted +
                ", numItems = " + numItems +
                ", p: " + p +
                ", q1: " + q1 +
                ", q2: " + q2 +
                ", sumOfWeights: " + sumOfWeights +
                ", upperBound: " + upperBound +
                ", fromMappers: " + fromMappers);

        return new CombinerOutput(combinerId, mapperOutputs, combinerSelectedSamples, combinerAcceptedSamples, numItems, numRejectedItems + rejected, rejected, sumOfWeights, useGlobalUpperBound, upperBound, q1, q2);
    }

    //Simulate reducer
    ReducerOutput reduce(String reducerId, List<CombinerOutput> combinerOutputs, double p, double err) {
        int numItems = 0;
        int numRejectedItems = 0;
        double upperBound = 0;
        double sumOfWeights = 0;
        boolean useGlobalUpperBound = false;

        List<Sample> reducerSelectedSamples = new ArrayList<Sample>();

        List<Sample> aggCombinerSelectedSamples = new ArrayList<Sample>();

        List<Sample> reducerAcceptedSamples = new ArrayList<Sample>();

        for (CombinerOutput combinerOutput : combinerOutputs) {
            numItems += combinerOutput.numItems;
            numRejectedItems += combinerOutput.numRejectedItems;
            sumOfWeights += combinerOutput.sumOfWeights;
            useGlobalUpperBound |= combinerOutput.useGlobalUpperBound;
            upperBound = Math.max(upperBound, combinerOutput.upperBound);
            aggCombinerSelectedSamples.addAll(combinerOutput.selectedSamples);
            reducerAcceptedSamples.addAll(combinerOutput.acceptedSamples);
        }

        double q1 = calculateQ1(sumOfWeights, numItems, upperBound, p, err);

        double q2 = calculateQ2(sumOfWeights, numItems, upperBound, p, err);

        for (CombinerOutput combinerOutput : combinerOutputs) {
            if (Double.compare(q1, combinerOutput.q1) > 0) {
                throw new RuntimeException("q1 is increased in reducer: [" +
                          q1 + ", " + combinerOutput.q1 + "]");
            }
            if (Double.compare(q2, combinerOutput.q2) < 0) {
                throw new RuntimeException("q2 is decreased in reducer: [" +
                        q2 + ", " + combinerOutput.q2 + "]");
            }
        }

        int selected = 0;
        int accepted = 0;
        int rejected = 0;

        for (Sample sample: aggCombinerSelectedSamples) {
            double score = sample.calculateScore(p, upperBound);
            if (useGlobalUpperBound) {
                if (score < q2) {
                    /*
                    System.out.println("reducer accept sample id: " + sample.id +
                            ", weight: " + sample.weight +
                            ", score: " + score);
                    */
                    accepted++;
                    reducerAcceptedSamples.add(sample);
                    continue;
                }
            }
            if (score < q1) {
                /*
                System.out.println("reducer selected sample id: " + sample.id +
                        ", weight: " + sample.weight +
                        ", score: " + sample.score);
                */
                selected++;
                reducerSelectedSamples.add(sample);
            } else {
                rejected++;
            }
        }

        int s = (int)Math.ceil(p * numItems); // sample size

        List<String> fromCombiners = new ArrayList<String>();

        for(CombinerOutput combinerOutput : combinerOutputs) {
            fromCombiners.add(combinerOutput.id);
        }

        System.out.println("reducer[ + " + reducerId + "] selected: " + selected +
                ", rejected = " + rejected +
                ", accepted = " + accepted +
                ", p: " + p +
                ", q1: " + q1 +
                ", q2: " + q2 +
                ", numItems: " + numItems +
                ", numOfInputItems: " + (selected + rejected) +
                ", sumOfWeights: " + sumOfWeights +
                ", s: " + s +
                ", from combiners:" + fromCombiners);

        Collections.sort(reducerSelectedSamples, new SampleScoreComparator());

        List<Sample> finalOutputSamples = new ArrayList<Sample>(reducerSelectedSamples.size() + reducerAcceptedSamples.size());

        finalOutputSamples.addAll(reducerAcceptedSamples);

        for (Sample sample : reducerSelectedSamples) {
            if (finalOutputSamples.size() >= s) {
                break;
            }
            finalOutputSamples.add(sample);
        }

        return new ReducerOutput(reducerId, combinerOutputs, reducerSelectedSamples, reducerAcceptedSamples, finalOutputSamples, numItems, numRejectedItems + rejected, rejected, sumOfWeights, useGlobalUpperBound, upperBound, q1, q2);
    }

    private static List<Sample> genGaussianInputSamples(int sampleCount) {
        List<Sample> samples = new ArrayList<Sample>(sampleCount);

        Random random = new Random();

        for (int i = 0; i < sampleCount; i++) {
            samples.add(new Sample(i, 5.0 + random.nextGaussian()));
        }

        Collections.sort(samples, new SampleWeightComparator());

        for (int i = 0; i < sampleCount; i++) {
            samples.get(i).id = i;
            if (i > 0) {
                Assert.assertTrue(Double.compare(samples.get(i).weight, samples.get(i - 1).weight) >= 0);
            }
        }

        return samples;
    }

    private static List<Sample> genUniformInputSamples(int sampleCount) {
        List<Sample> samples = new ArrayList<Sample>(sampleCount);

        Random random = new Random();

        for (int i = 0; i < sampleCount; i++) {
            samples.add(new Sample(i, random.nextDouble() * sampleCount));
        }

        Collections.sort(samples, new SampleWeightComparator());

        for (int i = 0; i < sampleCount; i++) {
            samples.get(i).id = i;
            if (i > 0) {
                Assert.assertTrue(Double.compare(samples.get(i).weight, samples.get(i - 1).weight) >= 0);
            }
        }

        return samples;
    }

    private static List<Sample> genEqualProbSamples(int sampleCount) {
        List<Sample> samples = new ArrayList<Sample>(sampleCount);

        double weight = Math.random();

        for (int i = 0; i < sampleCount; i++) {
            samples.add(new Sample(i, weight));
        }

        return samples;
    }

    private static List<Sample> genInputSamples(int sampleCount, int sampleType) {
        switch (sampleType) {
            case GAUSSIAN_SAMPLE: return genGaussianInputSamples(sampleCount);
            case EQUAL_SAMPLE: return genEqualProbSamples(sampleCount);
            case UNIFORM_SAMPLE:
            default: return genUniformInputSamples(sampleCount);
        }
    }

    private static void generateBaselineWeightDistribution(List<Sample> samples,
            double p,
            double globalUpperBound,
            Map<Integer, Integer> baselineCountMap,
            Map<Integer, Sample> sampleMap) throws IOException {
        List<Sample> orderedSamples = new ArrayList<Sample>(samples.size());
        for (Sample sample : samples) {
            sample.calculateBaselineScore(globalUpperBound);
            orderedSamples.add(sample);
        }
        Collections.sort(orderedSamples, new SampleScoreComparator());
        int s = (int)Math.ceil(p * samples.size()); // sample size
        for (Sample sample: orderedSamples.subList(0, s)) {
            if (!baselineCountMap.containsKey(sample.id)) {
                baselineCountMap.put(sample.id, 0);
            }
            baselineCountMap.put(sample.id, baselineCountMap.get(sample.id) + 1);
        }
    }

    private static final int GAUSSIAN_SAMPLE = 0;
    private static final int UNIFORM_SAMPLE = 1;
    private static final int EQUAL_SAMPLE = 2;

    public static void main(String[] args) throws IOException {

        int sampleCount = Integer.parseInt(args[0]);

        double p = Double.parseDouble(args[1]);

        boolean useGlobalUpperbound = Boolean.parseBoolean(args[2]);

        int sampleWeightDistribution = Integer.parseInt(args[3]);

        String outputFile = args[4];

        String baselineOutputFile = args[5];

        double err = 0.00001;

        List<Sample> samples = genInputSamples(sampleCount, sampleWeightDistribution);

        Map<Integer, Integer> baselineCountMap = new HashMap<Integer, Integer>();
        for (Sample sample : samples) {
            baselineCountMap.put(sample.id, 0);
        }

        Map<Integer, Sample> sampleMap = new HashMap<Integer, Sample>();
        for (Sample sample: samples) {
            sampleMap.put(sample.id, sample);
        }

        Map<Integer, Integer> countMap = new HashMap<Integer, Integer>();

        for (Sample sample : samples) {
            countMap.put(sample.id, 0);
        }

        ProbabilityWeightedSampling sorter = new ProbabilityWeightedSampling();

        List<Integer> selectedCounts = new ArrayList<Integer>();
        List<Integer> rejectedCounts = new ArrayList<Integer>();
        List<Integer> acceptedCounts = new ArrayList<Integer>();
        List<Integer> reducerInputCounts = new ArrayList<Integer>();

        int mappers = 20;

        int inputNumItemsPerMapper = samples.size() / mappers;

        int combiners = 10;

        int reducers = 5;

        for (int iter = 0; iter < 100; iter++) {
            System.out.println("Iteration {" + iter + "}");
            double globalUpperBound = 0;
            if (iter > 0) {
                for (Sample sample : samples) {
                    sample.resetRandomSeed(Math.random());
                }
            }
            for (Sample sample: samples) {
                globalUpperBound = Math.max(globalUpperBound, sample.weight);
            }
            Collections.shuffle(samples);
            List<MapperOutput> combinerInputs = new ArrayList<MapperOutput>();
            int mapperId = 0;
            for (int i = 0; i < samples.size(); i += inputNumItemsPerMapper) {
                List<Sample> subSamples = samples.subList(i, Math.min(i + inputNumItemsPerMapper, samples.size()));
                MapperOutput mapperOutput = sorter.map("mapper-" + mapperId, subSamples, p, err, useGlobalUpperbound, globalUpperBound);
                combinerInputs.add(mapperOutput);
                mapperId++;
            }

            //simulate shuffle
            Collections.shuffle(combinerInputs);

            List<CombinerOutput> reducerInputs = new ArrayList<CombinerOutput>();

            int partitonsPerCombiner = combinerInputs.size() / combiners;

            int combinerId = 0;

            for (int i = 0; i < combinerInputs.size(); i += partitonsPerCombiner) {
                List<MapperOutput> combinerInput = combinerInputs.subList(i, Math.min(i + partitonsPerCombiner, combinerInputs.size()));
                CombinerOutput combinerOutput = sorter.combine("combiner-" + combinerId, combinerInput, p, err);
                reducerInputs.add(combinerOutput);
                combinerId++;
            }

            List<ReducerOutput> reducerOutputs = new ArrayList<ReducerOutput>();

            int partitionsPerReducer = reducerInputs.size() / reducers;

            int numRejectedItems = 0;

            int numAcceptedItems = 0;

            int numReducerInputItems = 0;

            int reducerId = 0;

            for (int i = 0; i < reducerInputs.size(); i += partitionsPerReducer) {
                List<CombinerOutput> reducerInput = reducerInputs.subList(i, Math.min(i + partitionsPerReducer, reducerInputs.size()));
                ReducerOutput reducerOutput = sorter.reduce("reducer-" + reducerId, reducerInput, p, err);
                reducerOutputs.add(reducerOutput);
                numRejectedItems += reducerOutput.numRejectedItems;
                numAcceptedItems += reducerOutput.acceptedSamples.size();
                numReducerInputItems += reducerOutput.selectedSamples.size() +
                                        reducerOutput.acceptedSamples.size() +
                                        reducerOutput.numRejectedInReducer;
                reducerId++;
            }

            List<Sample> outputSamples = new ArrayList<Sample>();

            for (ReducerOutput reducerOutput : reducerOutputs) {
                outputSamples.addAll(reducerOutput.finalOutputSamples);
            }

            for (Sample sample: outputSamples) {
                if (!countMap.containsKey(sample.id)) {
                    countMap.put(sample.id, 0);
                }
                countMap.put(sample.id, countMap.get(sample.id) + 1);
            }

            selectedCounts.add(outputSamples.size());
            rejectedCounts.add(numRejectedItems);
            reducerInputCounts.add(numReducerInputItems);
            acceptedCounts.add(numAcceptedItems);

            //generate baseline weight-frequency distribution
            generateBaselineWeightDistribution(samples,
                    p,
                    globalUpperBound,
                    baselineCountMap,
                    sampleMap);
        }

        FileWriter fw = new FileWriter(outputFile);

        for (Integer key : countMap.keySet()) {
            //System.out.println(sampleMap.get(key).weight + "\t" + countMap.get(key));
            fw.write(sampleMap.get(key).weight + "\t" + countMap.get(key));
            fw.write("\r\n");
        }

        int maxSelectedCount = 0;
        int minSelectedCount = samples.size();
        int avgSelectedCount = 0;

        for (Integer selectedCount : selectedCounts) {
            maxSelectedCount = Math.max(maxSelectedCount, selectedCount);
            minSelectedCount =  Math.min(minSelectedCount, selectedCount);
            avgSelectedCount += selectedCount;
        }

        System.out.println("selected\t" + maxSelectedCount +
                "\t" + minSelectedCount +
                "\t" + avgSelectedCount / selectedCounts.size());

        int maxRejetedCount = 0;
        int minRejectededCount = samples.size();
        int avgRejectedCount = 0;

        for (Integer rejectedCount : rejectedCounts) {
            maxRejetedCount = Math.max(maxRejetedCount, rejectedCount);
            minRejectededCount =  Math.min(minRejectededCount, rejectedCount);
            avgRejectedCount += rejectedCount;
        }

        System.out.println("rejected\t" + maxRejetedCount +
                "\t" + minRejectededCount +
                "\t" + avgRejectedCount / selectedCounts.size());

        int maxAcceptedCount = 0;
        int minAcceptededCount = samples.size();
        int avgAcceptedCount = 0;

        for (Integer acceptedCount : acceptedCounts) {
            maxAcceptedCount = Math.max(maxAcceptedCount, acceptedCount);
            minAcceptededCount =  Math.min(minAcceptededCount, acceptedCount);
            avgAcceptedCount += acceptedCount;
        }

        System.out.println("accepted\t" + maxAcceptedCount +
                "\t" + minAcceptededCount +
                "\t" + avgAcceptedCount / selectedCounts.size());

        int maxReducerInputCount = 0;
        int minReducerInputCount = samples.size();
        int avgReducerInputCount = 0;

        for (Integer reducerInputCount : reducerInputCounts) {
            maxReducerInputCount = Math.max(maxReducerInputCount, reducerInputCount);
            minReducerInputCount =  Math.min(minReducerInputCount, reducerInputCount);
            avgReducerInputCount += reducerInputCount;
        }

        System.out.println("reducer_input\t" + maxReducerInputCount +
                "\t" + minReducerInputCount +
                "\t" + avgReducerInputCount / selectedCounts.size());

        fw.close();

        FileWriter bfw = new FileWriter(baselineOutputFile);

        for (Integer key : baselineCountMap.keySet()) {
            //System.out.println(sampleMap.get(key).weight + "\t" + countMap.get(key));
            bfw.write(sampleMap.get(key).weight + "\t" + baselineCountMap.get(key));
            bfw.write("\r\n");
        }

        bfw.close();
    }
}

