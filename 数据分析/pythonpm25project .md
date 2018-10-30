
# 中国五个城市PM 2.5数据分析

## 简介

PM2.5的检测数值通常包含在环境部门和公司的空气质量报告中。 PM2.5是指大气颗粒物（PM），其直径小于或等于2.5微米。在环境科学中，特指悬浮在空气中的固体颗粒或液滴，是空气污染的主要来源之一。

## 观察数据

本数据包含有中国五个城市(北京、上海、成都、广州、沈阳)从2010/1/1-2015/12/31的空气和气象数据。数据中的缺失值被标记为NaN。数据列名的含义如下：

```
No: 行号
year: 年份
month: 月份
day: 日期
hour: 小时
season: 季节
PM: PM2.5浓度 (ug/m^3)
DEWP: 露点 (摄氏温度) 指在固定气压之下，空气中所含的气态水达到饱和而凝结成液态水所需要降至的温度。
TEMP: Temperature (摄氏温度)
HUMI: 湿度 (%)
PRES: 气压 (hPa)
cbwd: 组合风向
Iws: 累计风速 (m/s)
precipitation: 降水量/时 (mm)
Iprec: 累计降水量 (mm)
```

由于项目难度设定的问题，在后面的分析中我们暂时没有对气象数据的处理和分析，如果同学感兴趣的话可以自行探索。如果你有足够的能力，我们也欢迎你不采用项目模板中的代码，对数据自行进行分析~


**问题 1**：至少写下两个你感兴趣的问题，请确保这些问题能够由现有的数据进行回答。

（问题示例：1. 2012年-2015年上海市PM 2.5的数据在不同的月份有什么变化趋势？2. 哪个城市的PM 2.5的含量较低？）

**答案**：

**第一个问题：**2012年-2015年成都市PM 2.5的数据在不同的月份有什么变化趋势

**第二个问题：**广州市PM 2.5的春季的情况

## 数据评估

现在是时候由你自己来探索数据了。我们已经将数据文件提供给你，你无需自己进行下载任何其他文件。此数据由五个文件组成，分别为：
```
BeijingPM20100101_20151231.csv,
ChengduPM20100101_20151231.csv,
GuangzhouPM20100101_20151231.csv,
ShanghaiPM20100101_20151231.csv,
ShenyangPM20100101_20151231.csv
```


首先我们先运行下面的代码单元格来加载你将在分析中使用的所有数据包和函数。这些都是在数据分析中常用的第三方库，我们会在后面的正式课程中进行更加细致的教学。

如果处理的数据量特别大时，最好先从数据样本开始。这样更容易检查我们的数据整理步骤是否有效，因为我们完成代码所需的时间将更少。如果我们对整个过程的进展较为满意，那就可以着手整理整个数据集。（注：本数据集的数量并不是特别大，例如50 MB以上，因此我们并未从中提取样本，而是直接对整个数据集进行的分析。）

```python
import csv
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn
%matplotlib inline
```

以上海数据为例，我们先使用Pandas的read_csv函数导入第一个数据集，并使用head、info、describe方法来查看数据中的基本信息。


```python
Shanghai_data = pd.read_csv('ShanghaiPM20100101_20151231.csv')
```


```python
Shanghai_data.head()
```




<div>
<style>
    .dataframe thead tr:only-child th {
        text-align: right;
    }

    .dataframe thead th {
        text-align: left;
    }

    .dataframe tbody tr th {
        vertical-align: top;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>No</th>
      <th>year</th>
      <th>month</th>
      <th>day</th>
      <th>hour</th>
      <th>season</th>
      <th>PM_Jingan</th>
      <th>PM_US Post</th>
      <th>PM_Xuhui</th>
      <th>DEWP</th>
      <th>HUMI</th>
      <th>PRES</th>
      <th>TEMP</th>
      <th>cbwd</th>
      <th>Iws</th>
      <th>precipitation</th>
      <th>Iprec</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>0</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>59.48</td>
      <td>1026.1</td>
      <td>1.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>59.48</td>
      <td>1025.1</td>
      <td>1.0</td>
      <td>SE</td>
      <td>2.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>2</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-7.0</td>
      <td>59.21</td>
      <td>1025.1</td>
      <td>0.0</td>
      <td>SE</td>
      <td>4.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>3</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>63.94</td>
      <td>1024.0</td>
      <td>0.0</td>
      <td>SE</td>
      <td>5.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>5</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>63.94</td>
      <td>1023.0</td>
      <td>0.0</td>
      <td>SE</td>
      <td>8.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
  </tbody>
</table>
</div>



从运行结果可以看出，除了上面提到的数据列之外，上海数据中还包含有`PM_Jingan`和`PM_Xuhui`两个观测站点的数据。并且数据中PM2.5的这三列包含有缺失值“NaN”。

接下来我们可以查看一下数据的缺失情况，以及数据的类型：


```python
Shanghai_data.info()
```

    <class 'pandas.core.frame.DataFrame'>
    RangeIndex: 52584 entries, 0 to 52583
    Data columns (total 17 columns):
    No               52584 non-null int64
    year             52584 non-null int64
    month            52584 non-null int64
    day              52584 non-null int64
    hour             52584 non-null int64
    season           52584 non-null int64
    PM_Jingan        24700 non-null float64
    PM_US Post       34039 non-null float64
    PM_Xuhui         25189 non-null float64
    DEWP             52571 non-null float64
    HUMI             52571 non-null float64
    PRES             52556 non-null float64
    TEMP             52571 non-null float64
    cbwd             52572 non-null object
    Iws              52572 non-null float64
    precipitation    48575 non-null float64
    Iprec            48575 non-null float64
    dtypes: float64(10), int64(6), object(1)
    memory usage: 6.8+ MB


通过数据中的打印，我们可以看出本数据中包含有整型(int)、浮点型(float)、字符串型(str)等数据的格式。


```python
# print type of python object
print(type(Shanghai_data['cbwd'][1]))
```

    <class 'str'>


变量名`PM_US Post`中包含空格，这也可能对我们后续的分析造成一定的困扰。因为大多数命令中，都是默认以空格做为值与值之间的分隔符，而不是做为文件名的一部分。因此我们需要将变量名中的空格改为下划线:


```python
Shanghai_data.columns = [c.replace(' ', '_') for c in Shanghai_data.columns]
Shanghai_data.head()
```




<div>
<style>
    .dataframe thead tr:only-child th {
        text-align: right;
    }

    .dataframe thead th {
        text-align: left;
    }

    .dataframe tbody tr th {
        vertical-align: top;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>No</th>
      <th>year</th>
      <th>month</th>
      <th>day</th>
      <th>hour</th>
      <th>season</th>
      <th>PM_Jingan</th>
      <th>PM_US_Post</th>
      <th>PM_Xuhui</th>
      <th>DEWP</th>
      <th>HUMI</th>
      <th>PRES</th>
      <th>TEMP</th>
      <th>cbwd</th>
      <th>Iws</th>
      <th>precipitation</th>
      <th>Iprec</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>0</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>59.48</td>
      <td>1026.1</td>
      <td>1.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>59.48</td>
      <td>1025.1</td>
      <td>1.0</td>
      <td>SE</td>
      <td>2.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>2</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-7.0</td>
      <td>59.21</td>
      <td>1025.1</td>
      <td>0.0</td>
      <td>SE</td>
      <td>4.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>3</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>63.94</td>
      <td>1024.0</td>
      <td>0.0</td>
      <td>SE</td>
      <td>5.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>5</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>63.94</td>
      <td>1023.0</td>
      <td>0.0</td>
      <td>SE</td>
      <td>8.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
  </tbody>
</table>
</div>



其中season的数据类型为int，但实际上season并不是一个数字，而是一个分类变量，我们可以将season中的数值1,2,3,4转换为对应的季节（可以使用Pandas的map方法来实现）：


```python
Shanghai_data['season'] = Shanghai_data['season'].map({1:'Spring', 2:'Summer', 3:'Autumn', 4: 'Winter'})
Shanghai_data.head()
```




<div>
<style>
    .dataframe thead tr:only-child th {
        text-align: right;
    }

    .dataframe thead th {
        text-align: left;
    }

    .dataframe tbody tr th {
        vertical-align: top;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>No</th>
      <th>year</th>
      <th>month</th>
      <th>day</th>
      <th>hour</th>
      <th>season</th>
      <th>PM_Jingan</th>
      <th>PM_US_Post</th>
      <th>PM_Xuhui</th>
      <th>DEWP</th>
      <th>HUMI</th>
      <th>PRES</th>
      <th>TEMP</th>
      <th>cbwd</th>
      <th>Iws</th>
      <th>precipitation</th>
      <th>Iprec</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>0</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>59.48</td>
      <td>1026.1</td>
      <td>1.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>1</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>59.48</td>
      <td>1025.1</td>
      <td>1.0</td>
      <td>SE</td>
      <td>2.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>2</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-7.0</td>
      <td>59.21</td>
      <td>1025.1</td>
      <td>0.0</td>
      <td>SE</td>
      <td>4.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>3</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>63.94</td>
      <td>1024.0</td>
      <td>0.0</td>
      <td>SE</td>
      <td>5.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>5</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>-6.0</td>
      <td>63.94</td>
      <td>1023.0</td>
      <td>0.0</td>
      <td>SE</td>
      <td>8.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
  </tbody>
</table>
</div>



并且通过打印数据的总行数，与上方non-null的数据数量进行对比，我们可以发现一些数据中存在缺失值，以及缺失值的数量。比如`PM_Jingan`中的non-null非空数据行数为24700, 该数据集的总行数为52584条，该数据缺失的条数则为：27884。

说明：下面代码中用到的`dropna()`函数能够删除掉数据中的缺失值。


```python
# print the length of data
print("The number of row in this dataset is ",len(Shanghai_data.index))

# calculating the number of records in column "PM_Jingan"
print("There number of missing data records in PM_Jingan is: ", 
      len(Shanghai_data.index) - len(Shanghai_data['PM_Jingan'].dropna()))

```

    The number of row in this dataset is  52584
    There number of missing data records in PM_Jingan is:  27884


请选择一个你感兴趣的城市数据集，对你的数据进行类似于上方的评估，并回答下面的问题，说明你观察到的信息：


```python
# TO DO: fill in blanks below to load the city's data
city_data = pd.read_csv('ChengduPM20100101_20151231.csv')
```


```python
city_data.head()
```




<div>
<style>
    .dataframe thead tr:only-child th {
        text-align: right;
    }

    .dataframe thead th {
        text-align: left;
    }

    .dataframe tbody tr th {
        vertical-align: top;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>No</th>
      <th>year</th>
      <th>month</th>
      <th>day</th>
      <th>hour</th>
      <th>season</th>
      <th>PM_Caotangsi</th>
      <th>PM_Shahepu</th>
      <th>PM_US Post</th>
      <th>DEWP</th>
      <th>HUMI</th>
      <th>PRES</th>
      <th>TEMP</th>
      <th>cbwd</th>
      <th>Iws</th>
      <th>precipitation</th>
      <th>Iprec</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>0</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>4.0</td>
      <td>81.20</td>
      <td>1022.0</td>
      <td>7.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>4.0</td>
      <td>86.99</td>
      <td>1022.0</td>
      <td>6.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>2</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>4.0</td>
      <td>86.99</td>
      <td>1021.0</td>
      <td>6.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>3</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>86.89</td>
      <td>1021.0</td>
      <td>5.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>5</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>4</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>2.0</td>
      <td>86.79</td>
      <td>1021.0</td>
      <td>4.0</td>
      <td>cv</td>
      <td>1.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
  </tbody>
</table>
</div>




```python
city_data.isnull().sum()
```




    No                   0
    year                 0
    month                0
    day                  0
    hour                 0
    season               0
    PM_Caotangsi     28164
    PM_Shahepu       27990
    PM_US Post       23684
    DEWP               529
    HUMI               535
    PRES               521
    TEMP               527
    cbwd               521
    Iws                533
    precipitation     2955
    Iprec             2955
    dtype: int64



**问题2a**: 你选择的城市数据中包含哪几个PM 2.5的监测点。请说明它们的列名，不需要包含`PM_US Post`.

**答案**：1.PM_Caotangsi  2.PM_Shahepu 

**问题2b**: 你选择的城市数据中的哪些列存在缺失值？缺失值的数量分别是多少？

**答案**： PM_Caotangsi条目缺失数量为28164
        PM_Shahepu条目缺失数量为27990
        PM_US Post条目缺失数量为23684
        DEWP条目缺失数量为529
        HUMI条目缺失数量为535
        PRES条目缺失数量为521
        TEMP条目缺失数量为527
        cbwd条目缺失数量为521
        Iws条目缺失数量为533
        precipitation条目缺失数量为2955
        Iprec条目缺失数量为2955

## 数据整理


```python
files = ['BeijingPM20100101_20151231.csv',
       'ChengduPM20100101_20151231.csv',
       'GuangzhouPM20100101_20151231.csv',
       'ShanghaiPM20100101_20151231.csv',
       'ShenyangPM20100101_20151231.csv']

out_columns = ['No', 'year', 'month', 'day', 'hour', 'season', 'PM_US Post']
```


```python
# create a void dataframe
df_all_cities = pd.DataFrame()
```


```python
# iterate to write diffrent files
for inx, val in enumerate(files):
    df = pd.read_csv(val)
    df = df[out_columns]
    # create a city column
    df['city'] = val.split('P')[0]
    # map season 
    df['season'] = df['season'].map({1:'Spring', 2:'Summer', 3:'Autumn', 4: 'Winter'})
    # append each file and merge all files into one
    df_all_cities = df_all_cities.append(df)
```


```python
# replace the space in variable names with '_'
df_all_cities.columns = [c.replace(' ', '_') for c in df_all_cities.columns]
```

我们来简单查看一下处理后的数据，观察数据是否符合我们的要求：


```python
df_all_cities.head()
```




<div>
<style>
    .dataframe thead tr:only-child th {
        text-align: right;
    }

    .dataframe thead th {
        text-align: left;
    }

    .dataframe tbody tr th {
        vertical-align: top;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>No</th>
      <th>year</th>
      <th>month</th>
      <th>day</th>
      <th>hour</th>
      <th>season</th>
      <th>PM_US_Post</th>
      <th>city</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>0</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>Beijing</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>1</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>Beijing</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>2</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>Beijing</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>3</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>Beijing</td>
    </tr>
    <tr>
      <th>4</th>
      <td>5</td>
      <td>2010</td>
      <td>1</td>
      <td>1</td>
      <td>4</td>
      <td>Winter</td>
      <td>NaN</td>
      <td>Beijing</td>
    </tr>
  </tbody>
</table>
</div>



## 数据筛选

接下来我们将会对你在问题1中提出的两个问题进行更进一步的思考。

`df_all_cities`是我们建立的一个包含所有数据的Pandas Dataframe，考虑到我们的分析目标，我们可能会需要提取部分数据来针对我们感兴趣的具体问题进行分析。为了方便大家对数据进行探索，在下面我们定义了一个`filter_data`和`reading_stats`的函数，通过输入不同的条件(conditions),该函数可以帮助我们筛选出这部分的数据。


```python
def filter_data(data, condition):
    """
    Remove elements that do not match the condition provided.
    Takes a data list as input and returns a filtered list.
    Conditions should be a list of strings of the following format:
      '<field> <op> <value>'
    where the following operations are valid: >, <, >=, <=, ==, !=
    
    Example: ["duration < 15", "start_city == 'San Francisco'"]
    """

    # Only want to split on first two spaces separating field from operator and
    # operator from value: spaces within value should be retained.
    field, op, value = condition.split(" ", 2)
    
    # check if field is valid
    if field not in data.columns.values :
        raise Exception("'{}' is not a feature of the dataframe. Did you spell something wrong?".format(field))

    # convert value into number or strip excess quotes if string
    try:
        value = float(value)
    except:
        value = value.strip("\'\"")

    # get booleans for filtering
    if op == ">":
        matches = data[field] > value
    elif op == "<":
        matches = data[field] < value
    elif op == ">=":
        matches = data[field] >= value
    elif op == "<=":
        matches = data[field] <= value
    elif op == "==":
        matches = data[field] == value
    elif op == "!=":
        matches = data[field] != value
    else: # catch invalid operation codes
        raise Exception("Invalid comparison operator. Only >, <, >=, <=, ==, != allowed.")
    
    # filter data and outcomes
    data = data[matches].reset_index(drop = True)
    return data
```


```python
def reading_stats(data, filters = [], verbose = True):
    """
    Report number of readings and average PM2.5 readings for data points that meet
    specified filtering criteria.
    """

    n_data_all = data.shape[0]

    # Apply filters to data
    for condition in filters:
        data = filter_data(data, condition)

    # Compute number of data points that met the filter criteria.
    n_data = data.shape[0]

    # Compute statistics for PM 2.5 readings.
    pm_mean = data['PM_US_Post'].mean()
    pm_qtiles = data['PM_US_Post'].quantile([.25, .5, .75]).as_matrix()
    
    # Report computed statistics if verbosity is set to True (default).
    if verbose:
        if filters:
            print('There are {:d} readings ({:.2f}%) matching the filter criteria.'.format(n_data, 100. * n_data / n_data_all))
        else:
            print('There are {:d} reading in the dataset.'.format(n_data))

        print('The average readings of PM 2.5 is {:.2f} ug/m^3.'.format(pm_mean))
        print('The median readings of PM 2.5 is {:.2f} ug/m^3.'.format(pm_qtiles[1]))
        print('25% of readings of PM 2.5 are smaller than {:.2f} ug/m^3.'.format(pm_qtiles[0]))
        print('25% of readings of PM 2.5 are larger than {:.2f} ug/m^3.'.format(pm_qtiles[2]))
        seaborn.boxplot(data['PM_US_Post'], showfliers=False)
        plt.title('Boxplot of PM 2.5 of filtered data')
        plt.xlabel('PM_US Post (ug/m^3)')

    # Return three-number summary
    return data
```

在使用中，我们只需要调用`reading_stats`即可，我们在这个函数中调用了`filter_data`函数，因此并不需要我们直接操作`filter_data`函数。下面是对于该函数的一些提示。

`reading_stats`函数中包含有3个参数：

1. 第一个参数（必须）：需要被加载的 dataframe，数据将从这里开始分析。
2. 第二个参数（可选）：数据过滤器，可以根据一系列输入的条件(conditions)来过滤将要被分析的数据点。过滤器应作为一系列条件提供，每个条件之间使用逗号进行分割，并在外侧使用`""`将其定义为字符串格式，所有的条件使用`[]`包裹。每个单独的条件应该为包含三个元素的一个字符串：`'<field> <op> <value>'`（元素与元素之间需要有一个空格字符来作为间隔），`<op>`可以使用以下任意一个运算符：>、<、>=、<=、==、!=。数据点必须满足所有条件才能计算在内。例如，["city == 'Beijing'", "season == 'Spring'"] 仅保留北京市，季节为春天的数据。在第一个条件中, `<field>`是city，`<op>`是 ==, `<value>`是'Beijing',因为北京为字符串，所以加了单引号，它们三个元素之间分别添加一个空格。最后，这个条件需要使用双引号引用起来。这个例子中使用了两个条件，条件与条件之间使用逗号进行分割，这两个条件最后被放在`[]`之中。
3. 第三个参数（可选）：详细数据，该参数决定我们是否打印被选择的数据的详细统计信息。如果verbose = True，会自动打印数据的条数，以及四分位点，并绘制箱线图。如果verbose = False, 则只会返回筛选后的dataframe，不进行打印。

下面我们会使用上海市2012年-2015年的数据进行举例，探索数据中的一些问题。要获取这部分的数据，我们可以使用下面的函数来实现。


```python
df_test = reading_stats(df_all_cities, ["city == 'Shanghai'", "year >= 2012"])
```

    There are 35064 readings (13.34%) matching the filter criteria.
    The average readings of PM 2.5 is 52.88 ug/m^3.
    The median readings of PM 2.5 is 41.00 ug/m^3.
    25% of readings of PM 2.5 are smaller than 26.00 ug/m^3.
    25% of readings of PM 2.5 are larger than 67.00 ug/m^3.



![png](output_42_1.png)



```python
df_test.info()
```

    <class 'pandas.core.frame.DataFrame'>
    RangeIndex: 35064 entries, 0 to 35063
    Data columns (total 8 columns):
    No            35064 non-null int64
    year          35064 non-null int64
    month         35064 non-null int64
    day           35064 non-null int64
    hour          35064 non-null int64
    season        35064 non-null object
    PM_US_Post    33962 non-null float64
    city          35064 non-null object
    dtypes: float64(1), int64(5), object(2)
    memory usage: 2.1+ MB


从这里的分析我们可以看出，上海市2012-2015年的PM2.5数据有35064条记录，其中PM_US Post站点的记录有33962条，缺失的数量并不是很大。因为Pandas（我们使用的第三方库）在计算统计数字的时候，会自动排除掉缺失的数值，因此在这里我们没有对缺失值进行任何处理。

上海市在2012-2015年期间PM 2.5的值主要分布在0-125 ug/m^3这个区间，平均数值为52.88 ug/m^3，中位数为41.00 ug/m^3，有25%的读数小于26.00 ug/m^3，有25%的读数大于67.00 ug/m^3。

下面请你对你所感兴趣的问题进行分析，并回答下面的问题：（提示: 你需要对前面提出的两个问题都进行思考。）

**问题3a**: 要回答你前面的提出的两个问题，你需要分别筛选哪部分的数据？请具体说明。

**答案**：

**第一个问题：**上海市2012-2015年的PM2.5数据

**第二个问题：**广州市春天的PM2.5数据

**问题3b**：请使用上面给出的`reading_stats`函数来调用你所需要的数据，请在下面填写合适的条件（conditions）。提示：对应此函数的第二个参数。

**答案**：

**第一个问题：**["city == 'Chengdu'", "year >= 2012"])

**第二个问题：**["city == 'Guangzhou'", "season == 'Spring'"])




```python
# TO DO: First question
df1 = reading_stats(df_all_cities, ["city == 'Chengdu'", "year >= 2012"])
```

    There are 35064 readings (13.34%) matching the filter criteria.
    The average readings of PM 2.5 is 83.41 ug/m^3.
    The median readings of PM 2.5 is 68.00 ug/m^3.
    25% of readings of PM 2.5 are smaller than 44.00 ug/m^3.
    25% of readings of PM 2.5 are larger than 105.00 ug/m^3.



![png](output_46_1.png)



```python
# TO DO: Second question
df2 = reading_stats(df_all_cities, ["city == 'Guangzhou'", "season == 'Spring'"])
```

    There are 13248 readings (5.04%) matching the filter criteria.
    The average readings of PM 2.5 is 54.01 ug/m^3.
    The median readings of PM 2.5 is 43.00 ug/m^3.
    25% of readings of PM 2.5 are smaller than 25.00 ug/m^3.
    25% of readings of PM 2.5 are larger than 69.00 ug/m^3.



![png](output_47_1.png)


**问题3c**：你获取的数据分别包含多少条记录，统计特征如何？

**答案**：

**第一个问题：成都市在2012-2015年期间PM 2.5的值主要分布在0-200 ug/m^3这个区间，平均数值为83.41 ug/m^3，中位数为68.00 ug/m^3，有25%的读数小于44.00 ug/m^3，有25%的读数大于105.00ug/m^3。

**第二个问题：广州市春天PM 2.5的值主要分布在0-140 ug/m^3这个区间，平均数值为54.01 ug/m^3，中位数为43.00 ug/m^3，有25%的读数小于25.00 ug/m^3，有25%的读数大于66.00ug/m^3。

## 数据探索性分析和可视化

得到了想要的数据之后，接下来你可以对数据进行探索性分析和可视化了，并报告你的发现！在这部分我们同样为你提供了一个函数来对PM 2.5的观测平均值制作柱形图的可视化，下面是关于本函数使用方法的一些提示：

1. 第一个参数（必须）：筛选后数据的 dataframe，将从这里分析数据。
2. 第二个参数（必须）：数据分析进行的维度，在这里可以填入一个column_name，比如'season', 'month', 'hour'等，对数据进行分组分析。
3. 第三个参数（可选）：可视化中柱形的颜色，默认为蓝色，你也可以选择你喜爱的其他颜色，比如red，blue，green等。但是请尽量保证一份可视化报告中图表颜色的一致和整洁性。




```python
def univariate_plot(data, key = '', color = 'blue'):
    """
    Plot average PM 2.5 readings, given a feature of interest
    """
    
    # Check if the key exists
    if not key:
        raise Exception("No key has been provided. Make sure you provide a variable on which to plot the data.")
    if key not in data.columns.values :
        raise Exception("'{}' is not a feature of the dataframe. Did you spell something wrong?".format(key))

    # Create plot
    plt.figure(figsize=(8,6))
    data.groupby(key)['PM_US_Post'].mean().plot(kind = 'bar', color = color)
    plt.ylabel('PM 2.5 (ug/m^3)')
    plt.title('Average PM 2.5 Reading by {:s}'.format(key), fontsize =14)
    plt.show()
    return None
```

我们以上海市2012年之后不同月份的PM 2.5的观测平均值为例，使用`univariate_plot`函数绘制了可视化：


```python
univariate_plot(df_test, 'month', 'grey')
```


![png](output_53_0.png)


从本可视化中我们可以看出在较温暖的月份（6-10月）空气中的PM 2.5含量较低，而较寒冷的月份，比如（11-1月）空气中的PM 2.5含量较高。

接下来，你可以对你提出的问题进行探索了！请使用上面的函数探索一些不同的变量，并记录你发现的一些趋势。如果你想用其他方式或多个方式探索数据集，可自行创建更多的单元格。


```python
# TO DO: 
# please use univariate_plot to visualize your data 
univariate_plot(df1, 'month', 'red')
```


![png](output_56_0.png)


**问题 4a**：上述可视化有何有趣的趋势？是否能够回答你的第一个问题？（如果不能，请说明你需要什么信息来帮助你来回答问题）

**答案**：从本可视化中我们可以看出在较温暖的月份（6-9月）空气中的PM 2.5含量较低，而较寒冷的月份，比如（11-1月）空气中的PM 2.5含量较高。
       其中1月份最高


```python
# TO DO: 
# please use univariate_plot to visualize your data
univariate_plot(df2, 'year', 'blue')
```


![png](output_58_0.png)


**问题 4b**：上述可视化有何有趣的趋势？是否能够回答你的第二个问题？（如果不能，请说明你需要什么信息来帮助你来回答问题）

**答案**：前面关于这个问题描写的不够全面，应该是描述春季PM2.5含量的变化趋势，提取后的数据发现2010~2011的数据缺失，2012年的PM2.5含量达到最高值，2013~2015年逐年降低，2015年最低。但是个人理解univariate_plot(df2, 'year', 'blue')中传入参数为year依旧只是说明逐年的春夏秋冬四季的PM2.5的总量，并不代表只是春季PM2.5的含量，故对此数据处理还需要将代码进行进一步的细化。

```python

```
