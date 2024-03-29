#include <string.h>
#include <iostream>
#include "defs.h"
#include "Scaner.h"
#include "Error.h"

// ������ �������� ���� � �� �������
TypeLex Keyword[MAX_KEYW] = { "float", "double", "int",  "main", "do", "while", "return" };
int IndexKeyword[MAX_KEYW] = { TYPE_FLOAT, TYPE_DOUBLE, TYPE_INT, TYPE_MAIN, TYPE_DO, TYPE_WHILE, TYPE_RETURN };


void Scaner::GetData(char* FileName)
{
	// ������� ������ ������ �� �����

	char aa;

	FILE* in;
	errno_t err = fopen_s(&in, const_cast<char*>(FileName), "r");

	if (err != 0)
	{
		this->error.print(ERROR_FILE_NOT_EXIST);
		exit(ERROR_FILE_NOT_EXIST);
	}

	int i = 0;
	while (!feof(in))
	{
		fscanf_s(in, "%c", &aa);

		if (!feof(in)) t[i++] = aa;

		if (i >= MAX_TEXT - 1)
		{
			this->error.print(ERROR_FILE_TOO_LONG);
			break;
		}
	}

	// ��������� ���� ����� ������
	t[i] = '\0';
	fclose(in);
}

// ����������� ������ - ��������� ���� � ������������� ��������� = 0
Scaner::Scaner(char* FileName)
{
	GetData(FileName);
	PutUK(0);
	this->num_of_str = this->old_num_of_str = 1;
}



// ������������ ���������
void Scaner::PutUK(int i)
{
	uk = i;

	// ������� ������ �������� ������ ������
	this->num_of_str = this->old_num_of_str;
}

// �������� ���������
int Scaner::GetUK()
{
	this->old_num_of_str = this->num_of_str;
	return uk;
}


// ������� ��� ��������� ������ ������� ������
int Scaner::GetNumOfLine()
{
	return this->num_of_str;
}

int Scaner::Scan(TypeLex l)
{
	// ������� ������������

	// ������� ���� �������
	int i;
	for (i = 0; i < MAX_LEX; i++) l[i] = 0;
	i = 0;

	// ���� ��� �������� ��������, ������� ����������
	while (true)
	{

		// ���� ���� �������, ��������� � ��������
		// ���������� ��
		while ((t[uk] == ' ') || (t[uk] == '\n') || (t[uk] == '\t'))
		{
			if (t[uk] == '\n')
				num_of_str++;
			uk++;

		}

		// ���� ����������� ('/' � '/') ��� ('/' � '*')
		// ��������� ������������ ��� ������������� �����������
		if ((t[uk] == '/' && t[uk + 1] == '/') || (t[uk] == '/' && t[uk + 1] == '*'))
		{
			uk++;

			// ���� ������������ - ���� ����� ������ (�����������)
			if (t[uk] == '/')
			{
				uk++;
				while ((t[uk] != '\n') && (t[uk] != '\0')) uk++;
				continue;
			}

			// ���� �������������, � ����������� ����� ���� ����� �����������
			else if (t[uk] == '*')
			{
				uk++;

				while (true)
				{
					// ���������� �������, ������� ����� ������ �����������
					while ((t[uk] != '*') && (t[uk] != '\0'))
					{
						if (t[uk] == '\n')
							num_of_str++;

						uk++;
					}




					// ����� �� ����� - ���������� �� ��� ������ - ���������� ������ � ��������� ����
					if (t[uk] == '\0')
					{
						this->error.print(ERROR_MULTY_COMMENT, l , this->num_of_str);
						uk--;
						break;
					}

					uk++;
					// ���� ����������� �� ������� - ���������� ��� �������
					if (t[uk] != '/')
					{
						uk++;
						continue;
					}

					break;
				}
				uk++;
				continue;
			}
		}
		// ������� �� �����
		break;
	}

	// ���� ����� ������
	if (t[uk] == '\0')
	{
		l[0] = '\0';
		return TYPE_END;
	}

	// �����, ���� �� ����� �� ����� �����
	else
	{

		// ���� ��������� �����, ��������� ����� �����
		if ((t[uk] >= '0') && (t[uk] <= '9'))
		{
			bool is_long = false; // ���� ��� ��������

			l[i++] = t[uk++];
			while ((t[uk] >= '0') && (t[uk] <= '9'))
				if (i < MAX_LEX - 1)
					l[i++] = t[uk++];
				else
				{
					uk++;
					is_long = true; // ������������� ���� � true - ����� �������
				}

			// ���� ����� ������� - �������� ������ � ���������� ������
			if (is_long)
			{
				this->error.print(ERROR_INT_CONST_TOO_LONG);
				return TYPE_ERROR;
			}

			return TYPE_CONST_INT;
		}

		// �������� �� ����� ����� ��������� � ���� ������ ������� ������������
		else if (((t[uk] >= 'a') && (t[uk] <= 'z')) || ((t[uk] >= 'A') && (t[uk] <= 'Z')) || (t[uk] == '_'))
		{
			bool is_long = false;

			l[i++] = t[uk++];
			// ���������� �������
			while (((t[uk] >= '0') && (t[uk] <= '9')) ||
				((t[uk] >= 'a') && (t[uk] <= 'z')) ||
				((t[uk] >= 'A') && (t[uk] <= 'Z')) ||
				(t[uk] == '_'))
				if (i < MAX_LEX - 1) l[i++] = t[uk++]; else { uk++;  is_long = true; }

			// ���� �� �������� �������������
			// � � ��������� ��� ��������������, ����� ������� > MAX_LEX, 
			// � ��� �����������, ��������, ������ ��������� ��������
			// ����� ������� ��, �������, ��� ������������� ����
			if (is_long)
			{
				this->error.print(ERROR_IDENT_TOO_LONG);
				return TYPE_ERROR;
			}

			// �������� �� �������� �����
			for (int j = 0; j < MAX_KEYW; j++)
				// ���� ���� � ������ �������� ���� - ������� ��� �����
				if (strcmp(l, Keyword[j]) == 0)
					return IndexKeyword[j];

			// ���� �� �������� ����� - ���������� ��� "�������������"
			return TYPE_IDENT;
		}


		else if (t[uk] == '\'') {

			// �� ����� �������� �������� � ���������
			uk++;

			// ���� �� ����� �� ����� ������ ��� ������
			if ((t[uk] != '\0') && (t[uk] != '\n'))
				// ��������� ����� ������
				l[i++] = t[uk++];


			if (t[uk] != '\'')
			{
				this->error.print(ERROR_CHAR_CONST_NOT_CLOSED);
				return TYPE_ERROR;
			}

			uk++;
			return TYPE_CONST_CHAR;
		}


		// �������� �� ������ '<' ��� ���������� '<='
		else if (t[uk] == '<')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_LESS_EQUAL;
			}
			else return TYPE_LESS_THAN;
		}

		// �������� �� ������ '>' ��� ���������� '>='
		else if (t[uk] == '>')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_GREATER_EQUAL;
			}
			else return TYPE_GREATER_THAN;
		}

		// �������� �� ���������� �������� '!='
		else if (t[uk] == '!')
		{

			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_NOT_EQUAL;
			}
			else
			{
				this->error.print(ERROR_MSG_SYMBOL_NOT_ASSING);
				return TYPE_ERROR;
			}
		}

		// �������� �� ���������� �������� '+=' ��� '++', ��� ������ '+'
		else if (t[uk] == '+')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_ADD_ASSIGN;
			}
			else if (t[uk] == '+')
			{
				l[i++] = t[uk++];
				return TYPE_INCREMENT;
			}
			else return TYPE_ADD;
		}

		// �������� �� ���������� �������� '-=' ��� '--', ��� ������ '-'
		else if (t[uk] == '-')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_SUB_ASSIGN;
			}
			else if (t[uk] == '-')
			{
				l[i++] = t[uk++];
				return TYPE_DECREMENT;
			}
			else return TYPE_SUB;
		}

		// �������� �� ���������� �������� *= ��� ������ *
		else if (t[uk] == '*')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_MULT_ASSIGN;
			}
			else return TYPE_MULT;
		}

		// �������� �� ���������� �������� /= ������ /
		else if (t[uk] == '/')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_DIV_ASSIGN;
			}
			else return TYPE_DIV;
		}

		// �������� �� ���������� �������� '%=' ��� ������ '%' 
		else if (t[uk] == '%')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_MODULO_ASSIGN;
			}
			else return TYPE_MODULO;
		}

		// �������� �� ���������� �������� '==' ��� ������ '='
		else if (t[uk] == '=')
		{
			l[i++] = t[uk++];

			if (t[uk] == '=')
			{
				l[i++] = t[uk++];
				return TYPE_EQUAL_TO;
			}
			else return TYPE_ASSIGNMENT;
		}


		// �������������� �������� �� ������� ������, ����� � �������, �������
		else if (t[uk] == '(') { l[i++] = t[uk++]; return TYPE_OPENED_PARANTHESES; }
		else if (t[uk] == ')') { l[i++] = t[uk++]; return TYPE_CLOSED_PARANTHESES; }
		else if (t[uk] == '{') { l[i++] = t[uk++]; return TYPE_OPENED_BRACE; }
		else if (t[uk] == '}') { l[i++] = t[uk++]; return TYPE_CLOSED_BRACE; }
		else if (t[uk] == '[') { l[i++] = t[uk++]; return TYPE_OPENED_BRACKET; }
		else if (t[uk] == ']') { l[i++] = t[uk++]; return TYPE_CLOSED_BRACKET; }
		else if (t[uk] == ';') { l[i++] = t[uk++]; return TYPE_SEMICOLON; }
		else if (t[uk] == ',') { l[i++] = t[uk++]; return TYPE_COMMA; }


		// ���� �� ���������� ������ ����� �������� - �������� ������
		else
		{
			l[i++] = t[uk++];
			this->error.print(ERROR_UNKNOWN_SYMBOL);
			return TYPE_ERROR;
		}
	}
}



