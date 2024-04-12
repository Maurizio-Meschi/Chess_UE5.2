from ctypes import alignment
import pandas as pd
import openpyxl
import os


def Manage_data(path_dip_csv, writer, Player1_Name, Player2_Name):
    data = pd.read_csv(path_dip_csv, sep=",")

    find_sheet = path_dip_csv.split('/')
    sheet = find_sheet[len(find_sheet)-1].split('.')[0]

    Player1WinMove = round(data[data['Player1'] == 'Win']['Move'].mean(), 0)
    Player2WinMove = round(data[data['Player2'] == 'Win']['Move'].mean(), 0)

    #Player1DrawMove = round(data[data['Player1'] == 'Draw']['Move'].mean(), 0)
    #Player2DrawMove = round(data[data['Player2'] == 'Draw']['Move'].mean(), 0)

    Player1MaxWinMove = round(data[data['Player1'] == 'Win']['Move'].max(), 0)
    Player2MaxWinMove = round(data[data['Player2'] == 'Win']['Move'].max(), 0)

    Player1MinWinMove = round(data[data['Player1'] == 'Win']['Move'].min(), 0)
    Player2MinWinMove = round(data[data['Player2'] == 'Win']['Move'].min(), 0)

    Player1Data = data['Player1'].value_counts()
    Player2Data = data['Player2'].value_counts()
    TotalRow = data['Game'].count()

    try:
        Player1Win = Player1Data['Win']
        NumWinPlayer1 = round((Player1Win/TotalRow) * 100, 2)
    except KeyError:
        NumWinPlayer1 = 0
        pass

    try:
        Player2Win = Player2Data['Win']
        NumWinPlayer2 = round((Player2Win/TotalRow) * 100, 2)
    except KeyError:
        NumWinPlayer2 = 0
        pass

    try:
        Draw = Player1Data['Draw']
        NumDraw = round((Draw/TotalRow) * 100, 2)
    except KeyError:
        NumDraw = 0
        pass

    df1 = pd.DataFrame({'Game': data['Game'], Player1_Name: data['Player1'], Player2_Name : data['Player2'], 'Move': data['Move']})

    df2 = pd.DataFrame([[str(NumWinPlayer1) + '%', str(NumDraw)+ '%', Player1MaxWinMove, Player1WinMove, Player1MinWinMove], #Player1DrawMove], 
                        [str(NumWinPlayer2) + '%', str(NumDraw)+ '%', Player2MaxWinMove, Player2WinMove, Player2MinWinMove]], #Player2DrawMove]],
                        [Player1_Name, Player2_Name], 
                        ['Win', 'Draw', '#Moves to Win | Max', '#Moves to win | Mean', '#Moves to Win | Min']) #'#Moves to draw | Mean'])
    
    df1.to_excel(writer, sheet_name=sheet + ' Data', index=False)
    df2.to_excel(writer, sheet_name=sheet + ' Statistics',  index=True)

    excel_file = writer.book

    ws_data = excel_file[sheet + ' Data']
    ws_statistics = excel_file[sheet + ' Statistics']

    for sheet_name, ws in zip([sheet + ' Data', sheet + ' Statistics'], [ws_data, ws_statistics]):
        for col in ws.columns:
            max_length = 0
            column = col[0].column_letter
            for cell in col:
                try: 
                    if len(str(cell.value)) > max_length:
                        max_length = len(cell.value)
                except:
                    pass
            adjusted_width = (max_length + 2) * 1.2
            ws.column_dimensions[column].width = adjusted_width
        for row in ws.iter_rows():
            for cell in row:
                cell.alignment = openpyxl.styles.Alignment(horizontal='center', vertical='center')




path_array = ["../CSV/Minimax-Minimax.csv", "../CSV/Random-Minimax.csv", "../CSV/Random-Random.csv"]

script_dir = os.path.abspath(__file__)
parent_dir = os.path.dirname(script_dir)

with pd.ExcelWriter('Statistics.xlsx', engine='openpyxl') as writer:
    for path in path_array:
        words = path.split('-')

        Player1_Name = words[0].split('/')[2] + '(White)'
        Player2_Name = words[-1].split('.')[0] + '(Balck)'

        csv_path = os.path.join(parent_dir, path)
        Manage_data(csv_path, writer, Player1_Name, Player2_Name)

